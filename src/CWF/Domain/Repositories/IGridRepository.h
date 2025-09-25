#pragma once

#include <memory>
#include "../Entities/Grid.h"
#include "../ValueObjects/Position.h"

namespace cwf::domain
{

    class IGridRepository
    {
    public:
        virtual ~IGridRepository() = default;

        // Core repository methods
        virtual std::unique_ptr<Grid> loadGrid(const std::string &identifier) = 0;
        virtual void saveGrid(const Grid &grid, const std::string &identifier) = 0;
        virtual std::vector<std::string> listSavedGrids() = 0;
        virtual bool gridExists(const std::string &identifier) = 0;
        virtual void deleteGrid(const std::string &identifier) = 0;
    };

} // namespace cwf::domain