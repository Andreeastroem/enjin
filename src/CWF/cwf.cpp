#include "cwf.h"
#include <algorithm>
#include <random>
#include <stdexcept>

namespace cwf
{

    // TileRules implementation
    void TileRules::addConnection(TileId from, Direction dir, TileId to, Weight weight)
    {
        auto &connections = weightedConnections[from][static_cast<size_t>(dir)];

        // Check if connection already exists
        auto it = std::find_if(connections.begin(), connections.end(),
                               [to](const auto &pair)
                               { return pair.first == to; });

        if (it != connections.end())
        {
            // Update weight if connection exists
            it->second += weight;
        }
        else
        {
            // Add new connection
            connections.emplace_back(to, weight);
        }
    }

    std::vector<TileRules::TileId> TileRules::getValidConnections(TileId from, Direction dir) const
    {
        std::vector<TileId> result;
        auto it = weightedConnections.find(from);
        if (it != weightedConnections.end())
        {
            const auto &connections = it->second[static_cast<size_t>(dir)];
            result.reserve(connections.size());
            for (const auto &[tileId, weight] : connections)
            {
                result.push_back(tileId);
            }
        }
        return result;
    }

    std::vector<std::pair<TileRules::TileId, TileRules::Weight>>
    TileRules::getWeightedConnections(TileId from, Direction dir) const
    {
        auto it = weightedConnections.find(from);
        if (it != weightedConnections.end())
        {
            return it->second[static_cast<size_t>(dir)];
        }
        return {};
    }

    bool TileRules::canConnect(TileId from, Direction dir, TileId to) const
    {
        auto connections = getWeightedConnections(from, dir);
        return std::find_if(connections.begin(), connections.end(),
                            [to](const auto &pair)
                            { return pair.first == to; }) != connections.end();
    }

    void TileRules::learnPattern(const std::vector<std::vector<char>> &pattern)
    {
        if (pattern.empty() || pattern[0].empty())
            return;

        auto [charToId, idToChar] = createTileMapping(pattern);

        // Analyze pattern for adjacency rules
        for (size_t y = 0; y < pattern.size(); ++y)
        {
            for (size_t x = 0; x < pattern[y].size(); ++x)
            {
                TileId currentTile = charToId[pattern[y][x]];

                // Check each direction
                if (y > 0)
                { // North
                    TileId northTile = charToId[pattern[y - 1][x]];
                    addConnection(currentTile, Direction::NORTH, northTile);
                }
                if (x < pattern[y].size() - 1)
                { // East
                    TileId eastTile = charToId[pattern[y][x + 1]];
                    addConnection(currentTile, Direction::EAST, eastTile);
                }
                if (y < pattern.size() - 1)
                { // South
                    TileId southTile = charToId[pattern[y + 1][x]];
                    addConnection(currentTile, Direction::SOUTH, southTile);
                }
                if (x > 0)
                { // West
                    TileId westTile = charToId[pattern[y][x - 1]];
                    addConnection(currentTile, Direction::WEST, westTile);
                }
            }
        }
    }

    std::pair<std::unordered_map<char, TileRules::TileId>,
              std::unordered_map<TileRules::TileId, char>>
    TileRules::createTileMapping(const std::vector<std::vector<char>> &pattern)
    {
        std::unordered_map<char, TileId> charToId;
        std::unordered_map<TileId, char> idToChar;
        TileId nextId = 0;

        // Find unique characters and assign IDs
        for (const auto &row : pattern)
        {
            for (char c : row)
            {
                if (charToId.find(c) == charToId.end())
                {
                    charToId[c] = nextId;
                    idToChar[nextId] = c;
                    ++nextId;
                }
            }
        }

        return {charToId, idToChar};
    }

    // Tile implementation
    Tile::Tile() : currentState(-1), collapsed(false) {}

    void Tile::collapse(TileId state)
    {
        if (std::find(possibleStates.begin(), possibleStates.end(), state) == possibleStates.end())
        {
            throw std::runtime_error("Attempting to collapse to invalid state");
        }

        possibleStates.clear();
        possibleStates.push_back(state);
        currentState = state;
        collapsed = true;
    }

    void Tile::addPossibleState(TileId state)
    {
        if (!collapsed && std::find(possibleStates.begin(), possibleStates.end(), state) == possibleStates.end())
        {
            possibleStates.push_back(state);
        }
    }

    void Tile::removePossibleState(TileId state)
    {
        if (collapsed)
            return;

        auto it = std::find(possibleStates.begin(), possibleStates.end(), state);
        if (it != possibleStates.end())
        {
            possibleStates.erase(it);
        }

        // If only one state remains, collapse to it
        if (possibleStates.size() == 1)
        {
            collapse(possibleStates[0]);
        }
    }

    // Grid implementation
    Grid::Grid(size_t width, size_t height)
        : width(width), height(height), tiles(width * height),
          rng(std::random_device{}()) {}

    void Grid::initialize(const std::vector<Tile::TileId> &possibleStates, const TileRules &rules)
    {
        this->rules = rules;
        for (auto &tile : tiles)
        {
            for (auto state : possibleStates)
            {
                tile.addPossibleState(state);
            }
        }
    }

    Tile &Grid::getTile(size_t x, size_t y)
    {
        if (x >= width || y >= height)
        {
            throw std::out_of_range("Tile coordinates out of bounds");
        }
        return tiles[y * width + x];
    }

    const Tile &Grid::getTile(size_t x, size_t y) const
    {
        if (x >= width || y >= height)
        {
            throw std::out_of_range("Tile coordinates out of bounds");
        }
        return tiles[y * width + x];
    }

    std::pair<size_t, size_t> Grid::findMinEntropyTile() const
    {
        size_t minEntropy = std::numeric_limits<size_t>::max();
        std::vector<std::pair<size_t, size_t>> minEntropyTiles;

        for (size_t y = 0; y < height; ++y)
        {
            for (size_t x = 0; x < width; ++x)
            {
                const Tile &tile = getTile(x, y);
                if (!tile.isCollapsed())
                {
                    size_t entropy = tile.getEntropy();
                    if (entropy < minEntropy)
                    {
                        minEntropy = entropy;
                        minEntropyTiles.clear();
                        minEntropyTiles.emplace_back(x, y);
                    }
                    else if (entropy == minEntropy)
                    {
                        minEntropyTiles.emplace_back(x, y);
                    }
                }
            }
        }

        if (minEntropyTiles.empty())
        {
            throw std::runtime_error("No uncollapsed tiles found");
        }

        // Randomly select one of the minimum entropy tiles
        std::uniform_int_distribution<size_t> dist(0, minEntropyTiles.size() - 1);
        return minEntropyTiles[dist(rng)];
    }

    bool Grid::collapseStep()
    {
        if (isFullyCollapsed())
            return false;

        try
        {
            auto [x, y] = findMinEntropyTile();
            Tile &tile = getTile(x, y);

            // Get weighted states based on neighbors
            std::unordered_map<Tile::TileId, float> stateWeights;
            auto states = tile.getPossibleStates();

            // Initialize weights
            for (auto state : states)
            {
                stateWeights[state] = 1.0f;
            }

            // Apply neighbor weights
            for (int d = 0; d < static_cast<int>(Direction::COUNT); ++d)
            {
                auto dir = static_cast<Direction>(d);
                auto [nx, ny] = getNeighborPosition(x, y, dir);

                if (nx >= 0 && ny >= 0)
                {
                    const Tile &neighbor = getTile(nx, ny);
                    if (neighbor.isCollapsed())
                    {
                        auto neighborState = neighbor.getPossibleStates()[0];
                        auto oppositeDir = static_cast<Direction>((static_cast<int>(dir) + 2) % 4);

                        // Get weighted connections from the neighbor
                        auto connections = rules.getWeightedConnections(neighborState, oppositeDir);
                        for (const auto &[state, weight] : connections)
                        {
                            if (stateWeights.find(state) != stateWeights.end())
                            {
                                stateWeights[state] *= (1.0f + weight);
                            }
                        }
                    }
                }
            }

            // Create weighted distribution
            std::vector<Tile::TileId> weightedStates;
            std::vector<float> weights;

            for (const auto &[state, weight] : stateWeights)
            {
                weightedStates.push_back(state);
                weights.push_back(weight);
            }

            // Create discrete distribution for weighted random selection
            std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
            tile.collapse(weightedStates[dist(rng)]);

            // Propagate the constraints to neighboring tiles
            propagateConstraints(x, y);

            return true;
        }
        catch (const std::exception &e)
        {
            return false;
        }
    }

    bool Grid::isFullyCollapsed() const
    {
        return std::all_of(tiles.begin(), tiles.end(),
                           [](const Tile &tile)
                           { return tile.isCollapsed(); });
    }

    void Grid::draw(float cellSize, float offsetX, float offsetY) const
    {
        for (size_t y = 0; y < height; ++y)
        {
            for (size_t x = 0; x < width; ++x)
            {
                const Tile &tile = getTile(x, y);
                float posX = x * cellSize + offsetX;
                float posY = y * cellSize + offsetY;
                Rectangle destRect = {posX, posY, cellSize, cellSize};

                if (tile.isCollapsed())
                {
                    // Draw collapsed tile
                    auto state = tile.getPossibleStates()[0];
                    auto it = tileVisuals.find(state);
                    if (it != tileVisuals.end())
                    {
                        const auto &visual = it->second;
                        DrawRectangleRec(destRect, visual.color);
                        if (visual.texturePath)
                        {
                            // Load texture (in practice, you'd want to cache these)
                            Texture2D texture = LoadTexture(visual.texturePath);
                            DrawTexturePro(texture, visual.sourceRect, destRect,
                                           Vector2{0, 0}, 0.0f, WHITE);
                            UnloadTexture(texture); // In practice, don't load/unload every frame
                        }
                    }
                }
                else
                {
                    // Draw uncollapsed tile with entropy visualization
                    float entropy = static_cast<float>(tile.getEntropy());
                    float maxEntropy = static_cast<float>(tileVisuals.size());
                    float brightness = (maxEntropy - entropy) / maxEntropy;
                    Color color = {128, 128, 128, static_cast<unsigned char>(255 * brightness)};
                    DrawRectangleRec(destRect, color);

                    // Draw grid lines
                    DrawRectangleLinesEx(destRect, 1, BLACK);
                }
            }
        }
    }

    std::pair<int, int> Grid::getNeighborPosition(size_t x, size_t y, Direction dir) const
    {
        switch (dir)
        {
        case Direction::NORTH:
            return {x, y > 0 ? y - 1 : -1};
        case Direction::SOUTH:
            return {x, y < height - 1 ? y + 1 : -1};
        case Direction::WEST:
            return {x > 0 ? x - 1 : -1, y};
        case Direction::EAST:
            return {x < width - 1 ? x + 1 : -1, y};
        default:
            return {-1, -1};
        }
    }

    void Grid::propagateConstraints(size_t x, size_t y)
    {
        const Tile &sourceTile = getTile(x, y);
        if (!sourceTile.isCollapsed())
            return;

        // Get the current state of the collapsed tile
        auto sourceState = sourceTile.getPossibleStates()[0];

        // Check each direction
        for (int d = 0; d < static_cast<int>(Direction::COUNT); ++d)
        {
            auto dir = static_cast<Direction>(d);
            auto [nx, ny] = getNeighborPosition(x, y, dir);

            // Skip if neighbor is out of bounds
            if (nx < 0 || ny < 0)
                continue;

            Tile &neighborTile = getTile(nx, ny);
            if (neighborTile.isCollapsed())
                continue;

            // Get all valid states for the neighbor based on the rules
            auto validStates = rules.getValidConnections(sourceState, dir);

            // Update neighbor's possible states to only include valid ones
            auto currentStates = neighborTile.getPossibleStates();
            for (auto state : currentStates)
            {
                if (std::find(validStates.begin(), validStates.end(), state) == validStates.end())
                {
                    neighborTile.removePossibleState(state);
                }
            }
        }
    }

} // namespace cwf