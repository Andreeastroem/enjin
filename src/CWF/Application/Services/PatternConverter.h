#pragma once

#include "../../Domain/Entities/Pattern.h"
#include "../../pattern_loader.h"

namespace cwf::application
{
    class PatternConverter
    {
    public:
        static std::unique_ptr<domain::Pattern> convertToDomainPattern(const cwf::Pattern &pattern)
        {
            auto domainPattern = std::make_unique<domain::Pattern>();

            // Convert tile mappings
            for (const auto &[symbol, id] : pattern.tileMapping.charToId)
            {
                domainPattern->addTileMapping(symbol, domain::TileId(id));
            }

            // Convert pattern rules
            // Assuming rules are implicit in the charPattern
            for (size_t y = 0; y < pattern.charPattern.size(); ++y)
            {
                for (size_t x = 0; x < pattern.charPattern[y].size(); ++x)
                {
                    char currentSymbol = pattern.charPattern[y][x];
                    auto currentId = domainPattern->getTileId(currentSymbol);
                    if (!currentId)
                        continue;

                    // Check each neighbor direction
                    std::array<std::pair<int, int>, 4> offsets = {{{0, -1}, {1, 0}, {0, 1}, {-1, 0}}};
                    for (size_t dir = 0; dir < 4; ++dir)
                    {
                        int nx = x + offsets[dir].first;
                        int ny = y + offsets[dir].second;

                        if (ny >= 0 && ny < pattern.charPattern.size() &&
                            nx >= 0 && nx < pattern.charPattern[y].size())
                        {
                            char neighborSymbol = pattern.charPattern[ny][nx];
                            auto neighborId = domainPattern->getTileId(neighborSymbol);
                            if (neighborId)
                            {
                                domainPattern->addConnection(*currentId, static_cast<domain::Direction>(dir), *neighborId);
                            }
                        }
                    }
                }
            }

            return domainPattern;
        }
    };
}