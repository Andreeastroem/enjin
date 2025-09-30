#pragma once

#include "Domain/Entities/AvailableTiles.h"

namespace cwf::domain
{
    class IAvailableTilesRepository
    {
    public:
        virtual ~IAvailableTilesRepository() = default;

        virtual std::unique_ptr<AvailableTiles> loadTiles() = 0;
        virtual void saveAvailableTiles(const AvailableTiles tiles) = 0;
    };
}