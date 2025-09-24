#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

namespace cwf
{

    class PatternLoader
    {
    public:
        static std::vector<std::vector<char>> loadFromFile(const std::string &filename)
        {
            std::vector<std::vector<char>> pattern;
            std::ifstream file(filename);

            if (!file.is_open())
            {
                throw std::runtime_error("Could not open pattern file: " + filename);
            }

            std::string line;
            while (std::getline(file, line))
            {
                // Skip empty lines and comment lines
                if (line.empty() || line[0] == '#')
                    continue;

                // Add the line to the pattern
                pattern.emplace_back(line.begin(), line.end());
            }

            if (pattern.empty())
            {
                throw std::runtime_error("Pattern file is empty: " + filename);
            }

            // Verify that all rows have the same length
            size_t width = pattern[0].size();
            for (const auto &row : pattern)
            {
                if (row.size() != width)
                {
                    throw std::runtime_error("Inconsistent row lengths in pattern file: " + filename);
                }
            }

            return pattern;
        }
    };

} // namespace cwf
