#pragma once

#include <vector>

namespace cwf
{
    // Represents a single tile in the WFC grid
    class Tile
    {
        friend class TileWeights;

    public:
        using TileId = int;
        struct PossibleState
        {
            TileId id;
            float weight;
        };
        using TileMapCharacter = char;

        Tile();
        Tile(std::string name, TileId id);

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

        const TileId getCurrectState();

    private:
        std::vector<TileId> possibleStates;
        TileId currentState;
        bool collapsed;

    protected:
        std::string name;
        TileId id;
    };
}