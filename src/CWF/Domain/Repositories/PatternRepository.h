#pragma once
#include "IPatternRepository.h"

namespace cwf::domain
{
    class PatternRepository : public IPatternRepository
    {
    public:
        PatternRepository() = default;

        std::unique_ptr<Pattern> loadPattern(const std::string &id) override
        {
            // For now, return an empty pattern since we're using the pattern loader directly in main
            return std::make_unique<Pattern>();
        }

        void savePattern(const Pattern &pattern, const std::string &identifier) override
        {
            // No-op for now
        }

        std::vector<std::string> listAvailablePatterns() override
        {
            // For now, return empty list
            return {};
        }

        bool patternExists(const std::string &identifier) override
        {
            // For now, always return false
            return false;
        }

        void deletePattern(const std::string &identifier) override
        {
            // No-op for now
        }
    };
}