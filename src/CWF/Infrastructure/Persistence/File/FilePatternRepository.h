#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../../../Domain/Repositories/IPatternRepository.h"

namespace cwf::infrastructure
{
    class FilePatternRepository : public domain::IPatternRepository
    {
    public:
        explicit FilePatternRepository(const std::string &basePath);

        std::unique_ptr<domain::Pattern> loadPattern(const std::string &identifier) override;
        void savePattern(const domain::Pattern &pattern, const std::string &identifier) override;
        std::vector<std::string> listAvailablePatterns() override;
        bool patternExists(const std::string &identifier) override;
        void deletePattern(const std::string &identifier) override;

    private:
        std::string getFullPath(const std::string &identifier) const;
        std::vector<std::string> splitString(const std::string &str, char delim);
        std::string base_path_;
    };
} // namespace cwf::infrastructure