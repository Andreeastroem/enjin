#include "tile_weights.h"

#include <unordered_map>
#include "TileUtils.h"

#include <fstream>

namespace cwf
{
    struct Direction
    {
        int x;
        int y;
    };
    void TileWeights::calculateTileWeights(std::vector<std::vector<Tile>> tilemap)
    {
        // Directions NORTH (0, -1), EAST (1, 0), SOUTH (0, 1), WEST (-1, 0)
        std::vector<Direction> directions{{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

        for (uint16_t row = 0; row < tilemap.size(); ++row)
        {
            for (uint16_t column = 0; column < tilemap[row].size(); ++column)
            {
                for (size_t directionIndex = 0; directionIndex < directions.size(); ++directionIndex)
                {
                    // Add the type of tile to the json if not exists
                    auto &tileInformation = weights[tilemap[row][column].name];
                    if (tileInformation.is_null())
                    {
                        tileInformation["tileId"] = tilemap[row][column].id;
                        tileInformation["name"] = tilemap[row][column].name;

                        tileInformation["potentialNeighbours"] = json::array({{}, {}, {}, {}});
                    }

                    // Bounds check
                    auto direction = directions[directionIndex];
                    int xPos = column + direction.x;
                    int yPos = row + direction.y;

                    if (yPos < 0 || yPos > tilemap.size() - 1) // unsure if it should be -1
                    {
                        continue;
                    }

                    if (xPos < 0 || xPos > tilemap[row].size() - 1) // unsure if it should be -1
                    {
                        continue;
                    }

                    // Does the neighbourtype already exist in the direction?
                    auto neighbourTile = tilemap[yPos][xPos];
                    auto &neighbourTileCount = tileInformation["potentialNeighbours"].at(directionIndex)[neighbourTile.name];

                    if (neighbourTileCount.is_null())
                    {
                        neighbourTileCount = 1;
                    }
                    else
                    {
                        neighbourTileCount = neighbourTileCount.get<int>() + 1;
                    }
                }
            }
        }
        // Normalise the values to create a percentage distribution [0-1]
        for (auto &[tileName, tileInfo] : weights.items())
        {
            auto &potentialNeighbours = tileInfo["potentialNeighbours"];

            // Loop over each "direction" (can be null)
            for (auto &direction : potentialNeighbours)
            {
                if (direction.is_null())
                    continue; // skip null entries

                // First pass: sum all counts in this direction
                uint totalAmount = 0;
                for (auto &[neighbourName, amount] : direction.items())
                {
                    totalAmount += amount.get<uint>();
                }

                // Second pass: normalise each neighbour
                for (auto &[neighbourName, amount] : direction.items())
                {
                    double normalised = static_cast<double>(amount.get<uint>()) / totalAmount;
                    amount = normalised; // replace the raw count with a percentage
                }
            }
        }
    }

    void TileWeights::writeTileWeightsToFile(std::string filename)
    {
        std::ofstream outputStream(filename);

        outputStream << std::setw(4) << weights << std::endl;
    }
}