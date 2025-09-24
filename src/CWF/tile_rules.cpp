#include "tile_rules.h"

namespace cwf
{
    TileRules::TileRules()
    {
    }

    TileRules::TileRules(TileMapping tileMapping)
    {
        charToId = tileMapping.charToId;
        idToChar = tileMapping.idToChar;
    }

    void TileRules::addTileMapping(TileMapping tileMapping)
    {
        charToId = tileMapping.charToId;
        idToChar = tileMapping.idToChar;
    }

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
        if (charToId.size() > 0 || idToChar.size() > 0)
        {
            return {
                charToId, idToChar};
        }

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

    std::pair<std::unordered_map<char, TileRules::TileId>,
              std::unordered_map<TileRules::TileId, char>>
    TileRules::getTileMapping()
    {
        return {
            charToId, idToChar};
    }
}