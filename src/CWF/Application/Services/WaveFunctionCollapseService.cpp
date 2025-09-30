#include "WaveFunctionCollapseService.h"
#include <stdexcept>
#include <algorithm>
#include <set>

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
        // First pass: gather all unique tile IDs from the pattern
        std::set<domain::TileId> allPossibleStates;

        // Loop through all possible pairs of tiles and directions
        for (const auto &[fromId, connections] : pattern.getAllConnections())
        {
            allPossibleStates.insert(fromId);
            for (int dir = 0; dir < 4; ++dir)
            {
                for (const auto &conn : connections[dir])
                {
                    allPossibleStates.insert(conn.toTile);
                }
            }
        }

        // Initialize each tile with all possible states
        for (int y = 0; y < grid.height(); ++y)
        {
            for (int x = 0; x < grid.width(); ++x)
            {
                domain::Position pos(x, y);
                auto &tile = grid.getTile(pos);

                for (const auto &state : allPossibleStates)
                {
                    tile.addPossibleState(state);
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

        // Calculate weights for each possible state based on neighbor constraints
        std::vector<float> weights;
        weights.reserve(possibleStates.size());

        for (const auto &state : possibleStates)
        {
            float weight = 1.0f;
            // Check each direction
            for (int dir = 0; dir < 4; ++dir)
            {
                auto neighborPos = grid.getNeighborPosition(
                    *minEntropyPos, static_cast<domain::Direction>(dir));

                if (neighborPos)
                {
                    const auto &neighborTile = grid.getTile(*neighborPos);
                    if (!neighborTile.isCollapsed())
                    {
                        // Count how many valid connections this state has with the neighbor's possible states
                        int validConnections = 0;
                        for (const auto &neighborState : neighborTile.possibleStates())
                        {
                            if (pattern.canConnect(state, static_cast<domain::Direction>(dir), neighborState))
                            {
                                validConnections++;
                            }
                        }
                        // Adjust weight based on valid connections
                        if (validConnections > 0)
                        {
                            weight *= static_cast<float>(validConnections) / neighborTile.possibleStates().size();
                        }
                        else
                        {
                            weight = 0.0f;
                            break;
                        }
                    }
                }
            }
            weights.push_back(weight);
        }

        // Check if we have any valid states
        float totalWeight = std::accumulate(weights.begin(), weights.end(), 0.0f);
        if (totalWeight <= 0.0f)
        {
            return false; // No valid states available
        }

        // Create distribution based on weights
        std::uniform_real_distribution<float> dist(0.0f, totalWeight);
        float selection = dist(rng_);

        // Select state based on weights
        float accumulator = 0.0f;
        size_t selectedIndex = 0;
        for (size_t i = 0; i < weights.size(); ++i)
        {
            accumulator += weights[i];
            if (accumulator > selection)
            {
                selectedIndex = i;
                break;
            }
        }

        // Collapse the tile
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
        std::vector<domain::Position> stack;
        std::vector<domain::Position> processed;

        // Add initial position's neighbors
        for (int dir = 0; dir < 4; ++dir)
        {
            if (auto neighborPos = grid.getNeighborPosition(pos, static_cast<domain::Direction>(dir)))
            {
                stack.push_back(*neighborPos);
            }
        }

        while (!stack.empty())
        {
            domain::Position currentPos = stack.back();
            stack.pop_back();

            // Skip if already processed
            if (std::find(processed.begin(), processed.end(), currentPos) != processed.end())
            {
                continue;
            }
            processed.push_back(currentPos);

            auto &currentTile = grid.getTile(currentPos);
            if (currentTile.isCollapsed())
            {
                continue;
            }

            std::vector<domain::TileId> validStates;
            const auto &currentPossibleStates = currentTile.possibleStates();

            for (const auto &state : currentPossibleStates)
            {
                bool isValid = true;

                // Check all directions
                for (int dir = 0; dir < 4 && isValid; ++dir)
                {
                    auto neighborPos = grid.getNeighborPosition(
                        currentPos, static_cast<domain::Direction>(dir));

                    if (!neighborPos)
                    {
                        continue; // Edge of grid
                    }

                    const auto &neighborTile = grid.getTile(*neighborPos);
                    bool hasValidConnection = false;

                    // For collapsed neighbors, we only need to check one state
                    if (neighborTile.isCollapsed())
                    {
                        hasValidConnection = pattern.canConnect(
                            state,
                            static_cast<domain::Direction>(dir),
                            neighborTile.currentState());
                    }
                    else
                    {
                        // For uncollapsed neighbors, check all possible states
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
                    }

                    if (!hasValidConnection)
                    {
                        isValid = false;
                    }
                }

                if (isValid)
                {
                    validStates.push_back(state);
                }
            }

            if (validStates.size() < currentTile.entropy())
            {
                // Create a new vector of states to remove
                std::vector<domain::TileId> statesToRemove;
                for (const auto &state : currentPossibleStates)
                {
                    if (std::find(validStates.begin(), validStates.end(), state) == validStates.end())
                    {
                        statesToRemove.push_back(state);
                    }
                }

                // Remove invalid states
                for (const auto &state : statesToRemove)
                {
                    currentTile.removePossibleState(state);
                }

                // Add neighbors to stack for processing
                for (int dir = 0; dir < 4; ++dir)
                {
                    if (auto neighborPos = grid.getNeighborPosition(
                            currentPos, static_cast<domain::Direction>(dir)))
                    {
                        // Only add if not already processed
                        if (std::find(processed.begin(), processed.end(), *neighborPos) == processed.end())
                        {
                            stack.push_back(*neighborPos);
                        }
                    }
                }
            }
        }
    }

} // namespace cwf::application