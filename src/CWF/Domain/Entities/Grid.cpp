#include "Grid.h"
#include <algorithm>
#include <limits>
#include <stdexcept>

namespace cwf::domain
{

    Grid::Grid(int width, int height)
        : width_(width), height_(height), tiles_(width * height) {}

    const Tile &Grid::getTile(const Position &pos) const
    {
        validatePosition(pos);
        return tiles_[positionToIndex(pos)];
    }

    Tile &Grid::getTile(const Position &pos)
    {
        validatePosition(pos);
        return tiles_[positionToIndex(pos)];
    }

    std::optional<Position> Grid::getNeighborPosition(const Position &pos, Direction dir) const
    {
        Position neighbor = calculateNeighborPosition(pos, dir);
        if (isValidPosition(neighbor))
        {
            return neighbor;
        }
        return std::nullopt;
    }

    bool Grid::isFullyCollapsed() const
    {
        return std::all_of(tiles_.begin(), tiles_.end(),
                           [](const Tile &tile)
                           { return tile.isCollapsed(); });
    }

    std::optional<Position> Grid::findLowestEntropyPosition() const
    {
        if (isFullyCollapsed())
        {
            return std::nullopt;
        }

        Position lowest_entropy_pos(0, 0);
        size_t lowest_entropy = std::numeric_limits<size_t>::max();

        for (int y = 0; y < height_; ++y)
        {
            for (int x = 0; x < width_; ++x)
            {
                Position current_pos(x, y);
                const Tile &tile = getTile(current_pos);

                if (!tile.isCollapsed() && tile.entropy() < lowest_entropy)
                {
                    lowest_entropy = tile.entropy();
                    lowest_entropy_pos = current_pos;
                }
            }
        }

        return lowest_entropy_pos;
    }

    int Grid::width() const { return width_; }

    int Grid::height() const { return height_; }

    bool Grid::isValidPosition(const Position &pos) const
    {
        return pos.x() >= 0 && pos.x() < width_ &&
               pos.y() >= 0 && pos.y() < height_;
    }

    void Grid::validatePosition(const Position &pos) const
    {
        if (!isValidPosition(pos))
        {
            throw std::out_of_range("Position is outside grid boundaries");
        }
    }

    Position Grid::calculateNeighborPosition(const Position &pos, Direction dir) const
    {
        switch (dir)
        {
        case Direction::NORTH:
            return Position(pos.x(), pos.y() - 1);
        case Direction::SOUTH:
            return Position(pos.x(), pos.y() + 1);
        case Direction::EAST:
            return Position(pos.x() + 1, pos.y());
        case Direction::WEST:
            return Position(pos.x() - 1, pos.y());
        default:
            return pos;
        }
    }

    size_t Grid::positionToIndex(const Position &pos) const
    {
        return pos.y() * width_ + pos.x();
    }

} // namespace cwf::domain