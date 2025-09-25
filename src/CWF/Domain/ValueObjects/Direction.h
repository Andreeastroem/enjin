#pragma once

namespace cwf::domain
{

    enum class Direction
    {
        NORTH,
        EAST,
        SOUTH,
        WEST
    };

    constexpr Direction oppositeDirection(Direction dir);

} // namespace cwf::domain