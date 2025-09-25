#include "Direction.h"

namespace cwf::domain
{

    constexpr Direction oppositeDirection(Direction dir)
    {
        switch (dir)
        {
        case Direction::NORTH:
            return Direction::SOUTH;
        case Direction::SOUTH:
            return Direction::NORTH;
        case Direction::EAST:
            return Direction::WEST;
        case Direction::WEST:
            return Direction::EAST;
        default:
            return Direction::NORTH; // Should never happen
        }
    }

} // namespace cwf::domain