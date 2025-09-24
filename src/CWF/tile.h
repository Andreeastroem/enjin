#pragma once

#include <vector>

namespace cwf
{
    // Represents a single tile in the WFC grid
    class Tile
    {
    public:
        using TileId = int;
        using TileMapCharacter = char;

        Tile();

        // Get the current entropy (number of possible states)
        size_t getEntropy() const { return possibleStates.size(); }

        // Collapse this tile to a specific state
        void collapse(TileId state);

        // Check if this tile has been collapsed
        bool isCollapsed() const { return collapsed; }

        // Add a possible state to this tile
        void addPossibleState(TileId state);

        // Remove a possible state from this tile
        void removePossibleState(TileId state);

        // Get the current possible states
        const std::vector<TileId> &getPossibleStates() const { return possibleStates; }

    private:
        std::vector<TileId> possibleStates;
        TileId currentState;
        bool collapsed;
    };
}