#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

#include "TileUtils.h"

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

        static Pattern loadPatternFromFile(const std::string &filename)
        {
            Pattern pattern = Pattern();

            std::ifstream file(filename);

            if (!file.is_open())
            {
                throw std::runtime_error("Could not open pattern file: " + filename);
            }

            std::string line;
            // Get tilemappings
            while (std::getline(file, line))
            {
                if (line.find("--") != std::string::npos)
                {
                    break;
                }
                std::vector<std::string> values = findAll(line, ':');

                pattern.tileMapping.charToId.insert({values.at(1).at(0), std::stoi(values.at(2))});
                pattern.tileMapping.idToChar.insert({std::stoi(values.at(2)), values.at(1).at(0)});
            }

            // Get pattern
            while (std::getline(file, line))
            {
                // Skip empty lines and comment lines
                if (line.empty() || line[0] == '#')
                    continue;

                pattern.charPattern.emplace_back(line.begin(), line.end());
            }

            return pattern;
        }

    private:
        static std::vector<std::string> findAll(std::string string, char character)
        {
            std::vector<std::string> values;
            size_t characterPosition;
            size_t prevCharacterPosition = 0;

            characterPosition = string.find(character, prevCharacterPosition);
            if (characterPosition != std::string::npos)
            {
                char *value = new char[characterPosition];
                string.copy(value, characterPosition);
                values.push_back(std::string(value));
                prevCharacterPosition = characterPosition;
            }

            do
            {
                // This will find the size between two characters Grass:G:1
                characterPosition = string.find(character, prevCharacterPosition + 1);

                if (characterPosition == std::string::npos)
                {
                    size_t length = (string.length() - 1) - prevCharacterPosition;
                    char *value = new char[length];
                    string.copy(value, length, prevCharacterPosition + 1);
                    values.push_back(std::string(value));
                }
                else
                {
                    size_t length = characterPosition - prevCharacterPosition - 1;
                    char *value = new char[characterPosition];
                    string.copy(value, length, prevCharacterPosition + 1);
                    values.push_back(std::string(value));

                    prevCharacterPosition = string.find(character, characterPosition);
                }
            } while (characterPosition != std::string::npos);

            return values;
        }
    };

} // namespace cwf
