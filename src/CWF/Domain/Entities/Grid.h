#pragma once

#include <vector>
#include <memory>
#include <optional>
#include "Tile.h"
#include "../ValueObjects/Position.h"
#include "../ValueObjects/Direction.h"

namespace cwf::domain
{

    class Grid
    {
    public:
        Grid(int width, int height);

        // Domain methods
        const Tile &getTile(const Position &pos) const;
        Tile &getTile(const Position &pos);

        std::optional<Position> getNeighborPosition(const Position &pos, Direction dir) const;
        bool isFullyCollapsed() const;
        std::optional<Position> findLowestEntropyPosition() const;

        int width() const;
        int height() const;

    private:
        bool isValidPosition(const Position &pos) const;
        void validatePosition(const Position &pos) const;
        Position calculateNeighborPosition(const Position &pos, Direction dir) const;
        size_t positionToIndex(const Position &pos) const;

    private:
        int width_;
        int height_;
        std::vector<Tile> tiles_;
    };

} // namespace cwf::domain