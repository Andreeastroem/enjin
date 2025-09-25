#include "WaveFunctionCollapseService.h"
#include <stdexcept>
#include <algorithm>

namespace cwf::application
{

    WaveFunctionCollapseService::WaveFunctionCollapseService(
        std::shared_ptr<domain::IGridRepository> gridRepo,
        std::shared_ptr<domain::IPatternRepository> patternRepo)
        : grid_repository_(std::move(gridRepo)),
          pattern_repository_(std::move(patternRepo)),
          rng_(std::random_device{}()) {}

    std::unique_ptr<domain::Grid> WaveFunctionCollapseService::generateGrid(
        const std::string &patternId,
        int width,
        int height)
    {
        // Dispatch start event
        domain::EventBus::getInstance().dispatch(
            domain::GridGenerationStartEvent(width, height, patternId));

        try
        {
            // Load the pattern
            auto pattern = pattern_repository_->loadPattern(patternId);
            if (!pattern)
            {
                throw std::runtime_error("Failed to load pattern: " + patternId);
            }

            // Create a new grid
            auto grid = std::make_unique<domain::Grid>(width, height);

            // Initialize all tiles with all possible states
            initializeGrid(*grid, *pattern);

            // Perform wave function collapse
            while (!grid->isFullyCollapsed())
            {
                if (!performCollapseStep(*grid, *pattern))
                {
                    domain::EventBus::getInstance().dispatch(
                        domain::GridGenerationCompleteEvent(false, "Failed to generate valid grid"));
                    throw std::runtime_error("Failed to generate valid grid");
                }
            }

            domain::EventBus::getInstance().dispatch(
                domain::GridGenerationCompleteEvent(true));

            return grid;
        }
        catch (const std::exception &e)
        {
            domain::EventBus::getInstance().dispatch(
                domain::GridGenerationCompleteEvent(false, e.what()));
            throw;
        }
    }

    void WaveFunctionCollapseService::saveGeneratedGrid(
        const domain::Grid &grid,
        const std::string &identifier)
    {
        grid_repository_->saveGrid(grid, identifier);
    }

    std::unique_ptr<domain::Grid> WaveFunctionCollapseService::loadGrid(
        const std::string &identifier)
    {
        return grid_repository_->loadGrid(identifier);
    }

    void WaveFunctionCollapseService::initializeGrid(
        domain::Grid &grid,
        const domain::Pattern &pattern)
    {
        for (int y = 0; y < grid.height(); ++y)
        {
            for (int x = 0; x < grid.width(); ++x)
            {
                domain::Position pos(x, y);
                auto &tile = grid.getTile(pos);

                // Add all possible states from the pattern
                for (const auto &mapping : pattern.getTileMappings())
                {
                    tile.addPossibleState(mapping.first);
                }
            }
        }
    }

    bool WaveFunctionCollapseService::performCollapseStep(
        domain::Grid &grid,
        const domain::Pattern &pattern)
    {
        // Find tile with minimum entropy
        auto minEntropyPos = grid.findLowestEntropyPosition();
        if (!minEntropyPos)
        {
            return true; // Grid is fully collapsed
        }

        auto &tile = grid.getTile(*minEntropyPos);

        // Get possible states and their weights
        const auto &possibleStates = tile.possibleStates();
        if (possibleStates.empty())
        {
            return false; // Contradiction
        }

        // Randomly select a state based on weights
        std::uniform_int_distribution<size_t> dist(0, possibleStates.size() - 1);
        size_t selectedIndex = dist(rng_);
        tile.collapse(possibleStates[selectedIndex]);

        // Propagate constraints
        propagateConstraints(grid, pattern, *minEntropyPos);

        return true;
    }

    void WaveFunctionCollapseService::propagateConstraints(
        domain::Grid &grid,
        const domain::Pattern &pattern,
        const domain::Position &pos)
    {
        std::vector<std::pair<domain::Position, domain::Direction>> stack;

        // Add initial neighbors
        for (int dir = 0; dir < 4; ++dir)
        {
            auto neighborPos = grid.getNeighborPosition(
                pos, static_cast<domain::Direction>(dir));
            if (neighborPos)
            {
                stack.emplace_back(*neighborPos, static_cast<domain::Direction>(dir));
            }
        }

        // Process the stack
        while (!stack.empty())
        {
            auto [currentPos, direction] = stack.back();
            stack.pop_back();

            auto &currentTile = grid.getTile(currentPos);
            if (currentTile.isCollapsed())
            {
                continue;
            }

            // Get valid states based on neighbors
            std::vector<domain::TileId> validStates;
            for (const auto &state : currentTile.possibleStates())
            {
                bool isValid = true;

                // Check constraints with all neighbors
                for (int dir = 0; dir < 4; ++dir)
                {
                    auto neighborPos = grid.getNeighborPosition(
                        currentPos, static_cast<domain::Direction>(dir));
                    if (!neighborPos)
                    {
                        continue;
                    }

                    const auto &neighborTile = grid.getTile(*neighborPos);
                    bool hasValidConnection = false;

                    for (const auto &neighborState : neighborTile.possibleStates())
                    {
                        if (pattern.canConnect(
                                state,
                                static_cast<domain::Direction>(dir),
                                neighborState))
                        {
                            hasValidConnection = true;
                            break;
                        }
                    }

                    if (!hasValidConnection)
                    {
                        isValid = false;
                        break;
                    }
                }

                if (isValid)
                {
                    validStates.push_back(state);
                }
            }

            size_t oldEntropy = currentTile.entropy();

            // Update possible states
            for (const auto &state : currentTile.possibleStates())
            {
                if (std::find(validStates.begin(), validStates.end(), state) == validStates.end())
                {
                    currentTile.removePossibleState(state);
                }
            }

            // If entropy changed, add neighbors to stack
            if (currentTile.entropy() < oldEntropy)
            {
                for (int dir = 0; dir < 4; ++dir)
                {
                    auto neighborPos = grid.getNeighborPosition(
                        currentPos, static_cast<domain::Direction>(dir));
                    if (neighborPos)
                    {
                        stack.emplace_back(*neighborPos, static_cast<domain::Direction>(dir));
                    }
                }
            }
        }
    }

} // namespace cwf::application