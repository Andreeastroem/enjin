#pragma once
#include "IGridRepository.h"

namespace cwf::domain
{
    class GridRepository : public IGridRepository
    {
    public:
        GridRepository() = default;

        std::unique_ptr<Grid> loadGrid(const std::string &id) override
        {
            // For now, return nullptr since we're not using grid persistence
            return nullptr;
        }

        void saveGrid(const Grid &grid, const std::string &id) override
        {
            // No-op for now since we're not using grid persistence
        }
    };
}