#include "FileGridRepository.h"
#include <fstream>
#include <filesystem>
#include <sstream>

namespace cwf::infrastructure
{
    FileGridRepository::FileGridRepository(const std::string &basePath, const domain::Pattern &pattern)
        : base_path_(basePath), pattern_(pattern) {}

    FileGridRepository::FileGridRepository(const std::string &basePath)
        : base_path_(basePath) {}

    void FileGridRepository::setPattern(const domain::Pattern &pattern)
    {
        pattern_ = pattern;
    }

    std::unique_ptr<domain::Grid> FileGridRepository::loadGrid(const std::string &identifier)
    {
        std::string fullPath = getFullPath(identifier);
        std::ifstream file(fullPath);

        if (!file.is_open())
        {
            throw std::runtime_error("Could not open grid file: " + fullPath);
        }

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line))
        {
            if (!line.empty())
            { // Skip empty lines
                lines.push_back(line);
            }
        }

        if (lines.empty())
        {
            throw std::runtime_error("Grid file is empty: " + fullPath);
        }

        int height = lines.size();
        int width = lines[0].length();

        auto grid = std::make_unique<domain::Grid>(width, height);

        // Read tile states
        for (int y = 0; y < height; ++y)
        {
            if (static_cast<size_t>(width) != lines[y].length())
            {
                throw std::runtime_error("Inconsistent line length in grid file: " + fullPath);
            }

            for (int x = 0; x < width; ++x)
            {
                domain::Position pos(x, y);
                char symbol = lines[y][x];

                auto tileId = pattern_.getTileId(symbol);
                if (!tileId)
                {
                    throw std::runtime_error("Unknown tile symbol '" + std::string(1, symbol) + "' in grid file: " + fullPath);
                }

                grid->getTile(pos).collapse(*tileId);
            }
        }

        return grid;
    }

    void FileGridRepository::saveGrid(const domain::Grid &grid, const std::string &identifier)
    {
        std::string fullPath = getFullPath(identifier);
        std::ofstream file(fullPath);

        if (!file.is_open())
        {
            throw std::runtime_error("Could not create grid file: " + fullPath);
        }

        for (int y = 0; y < grid.height(); ++y)
        {
            for (int x = 0; x < grid.width(); ++x)
            {
                const auto &tile = grid.getTile(domain::Position(x, y));

                if (!tile.isCollapsed())
                {
                    throw std::runtime_error("Cannot save grid with uncollapsed tiles");
                }

                auto symbol = pattern_.getSymbol(tile.currentState());
                if (!symbol)
                {
                    throw std::runtime_error("No symbol mapping for tile state: " +
                                             std::to_string(tile.currentState().value()));
                }

                file << *symbol;
            }
            file << '\n';
        }
    }

    std::vector<std::string> FileGridRepository::listSavedGrids()
    {
        std::vector<std::string> grids;
        for (const auto &entry : std::filesystem::directory_iterator(base_path_))
        {
            if (entry.path().extension() == ".grid")
            {
                grids.push_back(entry.path().stem().string());
            }
        }
        return grids;
    }

    bool FileGridRepository::gridExists(const std::string &identifier)
    {
        return std::filesystem::exists(getFullPath(identifier));
    }

    void FileGridRepository::deleteGrid(const std::string &identifier)
    {
        std::filesystem::remove(getFullPath(identifier));
    }

    std::string FileGridRepository::getFullPath(const std::string &identifier) const
    {
        return (std::filesystem::path(base_path_) / (identifier + ".grid")).string();
    }
} // namespace cwf::infrastructure