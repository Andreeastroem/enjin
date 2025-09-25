#include "FilePatternRepository.h"
#include <fstream>
#include <filesystem>
#include <sstream>

namespace cwf::infrastructure
{
    FilePatternRepository::FilePatternRepository(const std::string &basePath)
        : base_path_(basePath) {}

    std::unique_ptr<domain::Pattern> FilePatternRepository::loadPattern(const std::string &identifier)
    {
        std::string fullPath = getFullPath(identifier);
        std::ifstream file(fullPath);

        if (!file.is_open())
        {
            throw std::runtime_error("Could not open pattern file: " + fullPath);
        }

        auto pattern = std::make_unique<domain::Pattern>();

        // Read tile mappings
        std::string line;
        while (std::getline(file, line))
        {
            if (line == "--")
                break; // Separator between mappings and pattern

            // Parse mapping line (format: "name:symbol:id")
            auto parts = splitString(line, ':');
            if (parts.size() >= 3)
            {
                char symbol = parts[1][0];
                int id = std::stoi(parts[2]);
                pattern->addTileMapping(symbol, domain::TileId(id));
            }
        }

        // Read pattern and create connections
        std::vector<std::string> patternRows;
        while (std::getline(file, line))
        {
            if (!line.empty() && line[0] != '#')
            {
                patternRows.push_back(line);
            }
        }

        // Learn pattern connections
        for (size_t y = 0; y < patternRows.size(); ++y)
        {
            for (size_t x = 0; x < patternRows[y].size(); ++x)
            {
                char currentSymbol = patternRows[y][x];
                auto currentId = pattern->getTileId(currentSymbol);
                if (!currentId)
                    continue;

                // Check neighbors
                std::array<std::pair<int, int>, 4> offsets = {{{0, -1}, {1, 0}, {0, 1}, {-1, 0}}};
                for (size_t dir = 0; dir < 4; ++dir)
                {
                    int nx = x + offsets[dir].first;
                    int ny = y + offsets[dir].second;

                    if (ny >= 0 && ny < patternRows.size() &&
                        nx >= 0 && nx < patternRows[ny].size())
                    {
                        char neighborSymbol = patternRows[ny][nx];
                        auto neighborId = pattern->getTileId(neighborSymbol);
                        if (neighborId)
                        {
                            pattern->addConnection(*currentId, static_cast<domain::Direction>(dir), *neighborId);
                        }
                    }
                }
            }
        }

        return pattern;
    }

    void FilePatternRepository::savePattern(const domain::Pattern &pattern, const std::string &identifier)
    {
        std::string fullPath = getFullPath(identifier);
        std::ofstream file(fullPath);

        if (!file.is_open())
        {
            throw std::runtime_error("Could not create pattern file: " + fullPath);
        }

        // TODO: Implement pattern serialization
    }

    std::vector<std::string> FilePatternRepository::listAvailablePatterns()
    {
        std::vector<std::string> patterns;
        for (const auto &entry : std::filesystem::directory_iterator(base_path_))
        {
            if (entry.path().extension() == ".pattern")
            {
                patterns.push_back(entry.path().stem().string());
            }
        }
        return patterns;
    }

    bool FilePatternRepository::patternExists(const std::string &identifier)
    {
        return std::filesystem::exists(getFullPath(identifier));
    }

    void FilePatternRepository::deletePattern(const std::string &identifier)
    {
        std::filesystem::remove(getFullPath(identifier));
    }

    std::string FilePatternRepository::getFullPath(const std::string &identifier) const
    {
        return (std::filesystem::path(base_path_) / (identifier + ".pattern")).string();
    }

    std::vector<std::string> FilePatternRepository::splitString(const std::string &str, char delim)
    {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, delim))
        {
            result.push_back(item);
        }
        return result;
    }
} // namespace cwf::infrastructure