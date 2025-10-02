#pragma once

#include <nlohmann/json.hpp>

#include "tile.h"

using json = nlohmann::json;

namespace cwf
{
    class TileWeights
    {
    public:
        void calculateTileWeights(std::vector<std::vector<Tile>>);

    private:
        json weights;
    };
}