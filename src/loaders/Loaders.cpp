#include "loaders/Loaders.h"

#include <fstream>
#include <stdexcept>

#include "CWF/pattern_loader.h"
#include "CWF/tile_weights.h"

namespace loaders
{
    static cwf::Tile TileFromCharacter(char character)
    {
        switch (character)
        {
        case 'W':
            return cwf::Tile("Water", 2);
        case 'S':
            return cwf::Tile("Sand", 3);
        case 'T':
            return cwf::Tile("Tree", 4);
        case 'G':
        default:
            return cwf::Tile("Grass", 1);
        }
    }

    static bool IsValidCharacter(char character)
    {
        const char possibleValues[4] = {'G', 'S', 'W', 'T'};
        for (int i = 0; i < 4; ++i)
        {
            if (character == possibleValues[i])
                return true;
        }
        return false;
    }

    std::vector<std::vector<cwf::Tile>> ReadTileMapFromFile(const std::string &filename)
    {
        std::vector<std::vector<cwf::Tile>> tilemap;
        std::ifstream s(filename);
        std::string row;

        if (!s.is_open())
        {
            return tilemap;
        }

        while (std::getline(s, row))
        {
            std::vector<cwf::Tile> rowTiles;
            for (char tileCharacter : row)
            {
                if (IsValidCharacter(tileCharacter))
                {
                    cwf::Tile t = TileFromCharacter(tileCharacter);
                    rowTiles.push_back(t);
                }
            }
            if (!rowTiles.empty())
            {
                tilemap.push_back(rowTiles);
            }
        }

        s.close();
        return tilemap;
    }

    cwf::Pattern LoadPatternAndConfigureRules(const std::string &filename, cwf::TileRules &rules)
    {
        cwf::Pattern pattern;
        pattern = cwf::PatternLoader::loadPatternFromFile(filename);
        rules.addTileMapping(pattern.tileMapping);
        rules.learnPattern(pattern.charPattern);
        return pattern;
    }

    void LoadTilemapAndWriteWeights(const std::string &patternFilename, const std::string &outJsonFilename)
    {
        auto tilemap = ReadTileMapFromFile(patternFilename);
        cwf::TileWeights tileWeights;
        tileWeights.calculateTileWeights(tilemap);
        tileWeights.writeTileWeightsToFile(outJsonFilename);
    }
}
