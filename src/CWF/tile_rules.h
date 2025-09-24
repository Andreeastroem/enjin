#pragma once

#include <vector>
#include <unordered_map>

#include "TileUtils.h"

namespace cwf
{
    // Enum for directions (can be extended to 3D by adding UP and DOWN)
    enum class Direction
    {
        NORTH = 0,
        EAST = 1,
        SOUTH = 2,
        WEST = 3,
        COUNT = 4
    };

    // Class to manage the rules and weights for how tiles can connect
    class TileRules
    {
    public:
        using TileId = int;
        using Weight = float;

        TileRules();
        TileRules(TileMapping tileMapping);

        void addTileMapping(TileMapping tileMapping);

        // Add a valid connection between two tiles in a specific direction with a weight
        void addConnection(TileId from, Direction dir, TileId to, Weight weight = 1.0f);

        // Get all valid tiles that can connect in a specific direction with their weights
        std::vector<std::pair<TileId, Weight>> getWeightedConnections(TileId from, Direction dir) const;

        // Get all valid tiles that can connect in a specific direction
        std::vector<TileId> getValidConnections(TileId from, Direction dir) const;

        // Check if two tiles can connect in a specific direction
        bool canConnect(TileId from, Direction dir, TileId to) const;

        // Learn patterns from an example grid
        void learnPattern(const std::vector<std::vector<char>> &pattern);

        // Create a mapping between characters and tile IDs
        std::pair<std::unordered_map<char, TileId>, std::unordered_map<TileId, char>>
        createTileMapping(const std::vector<std::vector<char>> &pattern);

        std::pair<std::unordered_map<char, TileId>, std::unordered_map<TileId, char>> getTileMapping();

    private:
        // Maps a tile ID and direction to connecting tile IDs and their weights
        std::unordered_map<TileId, std::array<std::vector<std::pair<TileId, Weight>>,
                                              static_cast<size_t>(Direction::COUNT)>>
            weightedConnections;

        std::unordered_map<char, TileId> charToId;
        std::unordered_map<TileId, char> idToChar;
    };
}