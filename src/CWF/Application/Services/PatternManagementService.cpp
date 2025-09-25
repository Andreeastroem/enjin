#include "PatternManagementService.h"
#include <unordered_map>

namespace cwf::application
{

    PatternManagementService::PatternManagementService(
        std::shared_ptr<domain::IPatternRepository> patternRepo)
        : pattern_repository_(std::move(patternRepo)) {}

    std::unique_ptr<domain::Pattern> PatternManagementService::loadPattern(
        const std::string &identifier)
    {
        return pattern_repository_->loadPattern(identifier);
    }

    void PatternManagementService::savePattern(
        const domain::Pattern &pattern,
        const std::string &identifier)
    {
        pattern_repository_->savePattern(pattern, identifier);
    }

    std::vector<std::string> PatternManagementService::listAvailablePatterns()
    {
        return pattern_repository_->listAvailablePatterns();
    }

    void PatternManagementService::deletePattern(const std::string &identifier)
    {
        pattern_repository_->deletePattern(identifier);
    }

    std::unique_ptr<domain::Pattern> PatternManagementService::createPatternFromGrid(
        const domain::Grid &grid,
        const std::unordered_map<domain::TileId, char> &tileMapping)
    {
        auto pattern = std::make_unique<domain::Pattern>();

        // Add tile mappings
        for (const auto &[id, symbol] : tileMapping)
        {
            pattern->addTileMapping(symbol, id);
        }

        // Learn connections from the grid
        for (int y = 0; y < grid.height(); ++y)
        {
            for (int x = 0; x < grid.width(); ++x)
            {
                domain::Position currentPos(x, y);
                const auto &currentTile = grid.getTile(currentPos);

                if (!currentTile.isCollapsed())
                {
                    continue;
                }

                // Check all neighbors
                for (int dir = 0; dir < 4; ++dir)
                {
                    auto neighborPos = grid.getNeighborPosition(
                        currentPos, static_cast<domain::Direction>(dir));

                    if (!neighborPos)
                    {
                        continue;
                    }

                    const auto &neighborTile = grid.getTile(*neighborPos);
                    if (neighborTile.isCollapsed())
                    {
                        pattern->addConnection(
                            currentTile.currentState(),
                            static_cast<domain::Direction>(dir),
                            neighborTile.currentState());
                    }
                }
            }
        }

        return pattern;
    }

} // namespace cwf::application