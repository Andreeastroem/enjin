#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../Entities/Pattern.h"

namespace cwf::domain
{

    class IPatternRepository
    {
    public:
        virtual ~IPatternRepository() = default;

        // Core repository methods
        virtual std::unique_ptr<Pattern> loadPattern(const std::string &identifier) = 0;
        virtual void savePattern(const Pattern &pattern, const std::string &identifier) = 0;
        virtual std::vector<std::string> listAvailablePatterns() = 0;
        virtual bool patternExists(const std::string &identifier) = 0;
        virtual void deletePattern(const std::string &identifier) = 0;
    };

} // namespace cwf::domain