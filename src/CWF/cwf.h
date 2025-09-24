#pragma once

#include <vector>
#include <unordered_map>
#include <random>
#include <array>
#include "../clay_renderer/renderer.h"
#include "raylib.h"

namespace cwf
{

    // Structure to hold visualization settings for each tile type
    struct TileVisual
    {
        Color color;
        const char *texturePath; // Optional texture path
        Rectangle sourceRect;    // Source rectangle for texture
    };

    using TileVisuals = std::unordered_map<int, TileVisual>;

    // Forward declarations
    class Tile;
    class Grid;

    // Enum for directions (can be extended to 3D by adding UP and DOWN)
    enum class Direction
    {
        NORTH = 0,
        EAST = 1,
        SOUTH = 2,
        WEST = 3,
        COUNT = 4
    };

    // Structure to hold pattern analysis data
    struct PatternData
    {
        std::unordered_map<char, int> charToTileId;
        std::unordered_map<int, char> tileIdToChar;
        std::vector<std::vector<char>> pattern;
    };

    // Class to manage the rules and weights for how tiles can connect
    class TileRules
    {
    public:
        using TileId = int;
        using Weight = float;

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
        static std::pair<std::unordered_map<char, TileId>, std::unordered_map<TileId, char>>
        createTileMapping(const std::vector<std::vector<char>> &pattern);

    private:
        // Maps a tile ID and direction to connecting tile IDs and their weights
        std::unordered_map<TileId, std::array<std::vector<std::pair<TileId, Weight>>,
                                              static_cast<size_t>(Direction::COUNT)>>
            weightedConnections;
    };

    // Represents a single tile in the WFC grid
    class Tile
    {
    public:
        using TileId = int;

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

    // Represents the 2D grid of tiles
    // Can be extended to 3D by adding a depth dimension
    class Grid
    {
    public:
        Grid(size_t width, size_t height);

        // Initialize the grid with possible states and rules
        void initialize(const std::vector<Tile::TileId> &possibleStates, const TileRules &rules);

        // Get a tile at a specific position
        Tile &getTile(size_t x, size_t y);
        const Tile &getTile(size_t x, size_t y) const;

        // Get the position of a neighbor in a specific direction
        std::pair<int, int> getNeighborPosition(size_t x, size_t y, Direction dir) const;

        // Perform one step of the wave function collapse
        bool collapseStep();

        // Check if the entire grid has been collapsed
        bool isFullyCollapsed() const;

        // Get dimensions
        size_t getWidth() const { return width; }
        size_t getHeight() const { return height; }

        // Visualization methods
        void setVisuals(const TileVisuals &visuals) { tileVisuals = visuals; }
        void draw(float cellSize, float offsetX = 0, float offsetY = 0) const;

    private:
        // Find the tile with minimum entropy
        std::pair<size_t, size_t> findMinEntropyTile() const;

        // Propagate constraints after a tile collapse
        void propagateConstraints(size_t x, size_t y);

    private:
        std::vector<Tile> tiles;
        size_t width;
        size_t height;
        mutable std::mt19937 rng;
        TileRules rules;
        TileVisuals tileVisuals;
    };

} // namespace cwf
