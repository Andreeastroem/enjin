#pragma once

#include <string>
#include <vector>

#include "CWF/TileUtils.h"
#include "CWF/tile.h"
#include "CWF/tile_rules.h"

namespace loaders
{
    // Reads a simple char grid from a .pattern file and converts to tiles for weights.
    std::vector<std::vector<cwf::Tile>> ReadTileMapFromFile(const std::string &filename);

    // Loads a pattern file in the custom mapping+grid format and configures rules.
    // Returns the loaded pattern instance.
    cwf::Pattern LoadPatternAndConfigureRules(const std::string &filename, cwf::TileRules &rules);

    // Convenience: compute weights from file and write to JSON
    void LoadTilemapAndWriteWeights(const std::string &patternFilename, const std::string &outJsonFilename);
}
