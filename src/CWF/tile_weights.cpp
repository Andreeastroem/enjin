#include "tile_weights.h"

#include <unordered_map>
#include "TileUtils.h"

namespace cwf
{
    struct Direction
    {
        int x;
        int y;
    };
    void TileWeights::calculateTileWeights(std::vector<std::vector<Tile>> tilemap)
    {
        std::vector<Direction> directions{{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

        for (uint16_t x; x < tilemap.size(); ++x)
        {
            for (uint16_t y; y < tilemap[x].size(); ++y)
            {
                for (size_t directionIndex; directionIndex < directions.size(); ++directionIndex)
                {
                    // Add the type of tile to the json if not exists
                    auto &tileInformation = weights[tilemap[x][y].name];
                    if (tileInformation.is_null())
                    {
                        tileInformation["tileId"] = tilemap[x][y].id;
                        tileInformation["name"] = tilemap[x][y].name;

                        tileInformation["potentialNeighbours"] = json::array({{}, {}, {}, {}});
                    }

                    // Bounds check
                    auto direction = directions[directionIndex];
                    int xPos = x + direction.x;
                    int yPos = y + direction.y;

                    if (xPos < 0 || xPos > tilemap.size() - 1) // unsure if it should be -1
                    {
                        continue;
                    }

                    if (yPos < 0 || yPos > tilemap[x].size() - 1) // unsure if it should be -1
                    {
                        continue;
                    }

                    // Does the neighbourtype already exist in the direction?
                    auto neighbourTile = tilemap[xPos][yPos];
                    auto &neighbourTileCount = tileInformation["potentialNeighbours"].at(directionIndex)[neighbourTile.name];

                    if (neighbourTileCount.is_null())
                    {
                        neighbourTileCount = 1;
                    }
                    else
                    {
                        neighbourTileCount += 1;
                    }
                }
            }
        }
    }
}