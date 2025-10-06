#pragma once

#include <unordered_map>
#include <vector>
#include <random>
#include "raylib.h"

#include "tile.h"
#include "tile_rules.h"
#include "tile_weights.h"

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
        TileWeights weights;
    };
}