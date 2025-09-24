#pragma once

#include <unordered_map>
#include <vector>

namespace cwf
{
    using TileId = int;
    struct TileMapping
    {
        std::unordered_map<char, TileId> charToId;
        std::unordered_map<TileId, char> idToChar;
    };
    struct Pattern
    {
        TileMapping tileMapping;
        std::vector<std::vector<char>> charPattern;
    };
}