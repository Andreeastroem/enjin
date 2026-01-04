#include "grid.h"
#include <fstream>

namespace cwf
{
    Grid::Grid(size_t width, size_t height)
        : m_width(width), m_height(height), m_tiles(width * height),
          m_rng(std::random_device{}()) {}

    void Grid::initialize(const std::vector<Tile::TileId> &possibleStates, const TileRules &rules)
    {
        this->m_rules = rules;
        for (auto &tile : m_tiles)
        {
            for (auto state : possibleStates)
            {
                tile.addPossibleState(state);
            }
        }
    }

    Tile &Grid::getTile(size_t x, size_t y)
    {
        if (x >= m_width || y >= m_height)
        {
            throw std::out_of_range("Tile coordinates out of bounds");
        }
        return m_tiles[y * m_width + x];
    }

    const Tile &Grid::getTile(size_t x, size_t y) const
    {
        if (x >= m_width || y >= m_height)
        {
            throw std::out_of_range("Tile coordinates out of bounds");
        }
        return m_tiles[y * m_width + x];
    }

    std::pair<size_t, size_t> Grid::findMinEntropyTile() const
    {
        size_t minEntropy = std::numeric_limits<size_t>::max();
        std::vector<std::pair<size_t, size_t>> minEntropyTiles;

        for (size_t y = 0; y < m_height; ++y)
        {
            for (size_t x = 0; x < m_width; ++x)
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
        return minEntropyTiles[dist(m_rng)];
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
                        auto connections = m_rules.getWeightedConnections(neighborState, oppositeDir);
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
            tile.collapse(weightedStates[dist(m_rng)]);

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
        return std::all_of(m_tiles.begin(), m_tiles.end(),
                           [](const Tile &tile)
                           { return tile.isCollapsed(); });
    }

    void Grid::draw(float cellSize, float offsetX, float offsetY) const
    {
        for (size_t y = 0; y < m_height; ++y)
        {
            for (size_t x = 0; x < m_width; ++x)
            {
                const Tile &tile = getTile(x, y);
                float posX = x * cellSize + offsetX;
                float posY = y * cellSize + offsetY;
                Rectangle destRect = {posX, posY, cellSize, cellSize};

                if (tile.isCollapsed())
                {
                    // Draw collapsed tile
                    auto state = tile.getPossibleStates()[0];
                    auto it = m_tileVisuals.find(state);
                    if (it != m_tileVisuals.end())
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
                    float maxEntropy = static_cast<float>(m_tileVisuals.size());
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
            return {x, y < m_height - 1 ? y + 1 : -1};
        case Direction::WEST:
            return {x > 0 ? x - 1 : -1, y};
        case Direction::EAST:
            return {x < m_width - 1 ? x + 1 : -1, y};
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
            auto validStates = m_rules.getValidConnections(sourceState, dir);

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

    bool Grid::saveToFile(const std::string &fileName)
    {
        // Retrieve the mapping between tile IDs and their representative characters
        auto idToChar = m_rules.getTileMapping().second;

        try
        {
            std::ofstream s(fileName, std::ofstream::out | std::ofstream::trunc);
            if (!s.is_open())
            {
                return false;
            }

            // 1) Write mapping header lines: Name:Char:Id
            // We don't have human-readable names here, so we emit a placeholder name "Tile<id>".
            // The loader ignores the name field and only uses Char and Id.
            std::vector<std::pair<int, char>> orderedMappings;
            orderedMappings.reserve(idToChar.size());
            for (const auto &kv : idToChar)
            {
                orderedMappings.emplace_back(kv.first, kv.second);
            }
            std::sort(orderedMappings.begin(), orderedMappings.end(), [](const auto &a, const auto &b)
                      { return a.first < b.first; });

            for (const auto &[id, ch] : orderedMappings)
            {
                s << "Tile" << id << ":" << ch << ":" << id << "\n";
            }

            // Delimiter line
            s << "---\n";

            // 2) Write grid rows as characters
            for (size_t y = 0; y < m_height; ++y)
            {
                for (size_t x = 0; x < m_width; ++x)
                {
                    const Tile &tile = getTile(x, y);

                    // Determine the tile id to write
                    int idToWrite = -1;
                    if (tile.isCollapsed())
                    {
                        // Collapsed tile should have exactly one possible state
                        const auto &states = tile.getPossibleStates();
                        if (!states.empty())
                        {
                            idToWrite = states[0];
                        }
                    }
                    else
                    {
                        // If not collapsed, pick the first available state as a best-effort fallback
                        const auto &states = tile.getPossibleStates();
                        if (!states.empty())
                        {
                            idToWrite = states[0];
                        }
                    }

                    // Translate id to character (fallback to '?' if missing)
                    char tileCharacter = '?';
                    auto it = idToChar.find(idToWrite);
                    if (it != idToChar.end())
                    {
                        tileCharacter = it->second;
                    }

                    s.write(&tileCharacter, 1);
                }
                s << "\n";
            }

            s.close();
        }
        catch (const std::exception &error)
        {
            printf("%s", error.what());
            return false;
        }

        return true;
    }
}