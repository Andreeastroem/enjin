#pragma once

#include <memory>
#include <random>
#include "../../Domain/Entities/Grid.h"
#include "../../Domain/Entities/Pattern.h"
#include "../../Domain/Repositories/IGridRepository.h"
#include "../../Domain/Repositories/IPatternRepository.h"
#include "../../Domain/Events/EventDispatcher.h"
#include "../../Domain/Events/WaveFunctionEvents.h"

namespace cwf::application
{

    class WaveFunctionCollapseService
    {
    public:
        WaveFunctionCollapseService(
            std::shared_ptr<domain::IGridRepository> gridRepo,
            std::shared_ptr<domain::IPatternRepository> patternRepo);

        std::unique_ptr<domain::Grid> generateGrid(
            const std::string &patternId,
            int width,
            int height);

        void saveGeneratedGrid(
            const domain::Grid &grid,
            const std::string &identifier);

        std::unique_ptr<domain::Grid> loadGrid(
            const std::string &identifier);

    private:
        void initializeGrid(domain::Grid &grid, const domain::Pattern &pattern);
        bool performCollapseStep(domain::Grid &grid, const domain::Pattern &pattern);
        void propagateConstraints(
            domain::Grid &grid,
            const domain::Pattern &pattern,
            const domain::Position &pos);

    private:
        std::shared_ptr<domain::IGridRepository> grid_repository_;
        std::shared_ptr<domain::IPatternRepository> pattern_repository_;
        std::mt19937 rng_;
    };

} // namespace cwf::application