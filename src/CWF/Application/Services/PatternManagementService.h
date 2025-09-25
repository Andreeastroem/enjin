#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "../../Domain/Repositories/IPatternRepository.h"
#include "../../Domain/Entities/Pattern.h"
#include "../../Domain/Entities/Grid.h"
#include "../../Domain/ValueObjects/Position.h"

namespace cwf::application
{

    class PatternManagementService
    {
    public:
        explicit PatternManagementService(std::shared_ptr<domain::IPatternRepository> patternRepo);

        std::unique_ptr<domain::Pattern> loadPattern(const std::string &identifier);
        void savePattern(const domain::Pattern &pattern, const std::string &identifier);
        std::vector<std::string> listAvailablePatterns();
        void deletePattern(const std::string &identifier);

        std::unique_ptr<domain::Pattern> createPatternFromGrid(
            const domain::Grid &grid,
            const std::unordered_map<domain::TileId, char> &tileMapping);

    private:
        std::shared_ptr<domain::IPatternRepository> pattern_repository_;
    };

} // namespace cwf::application