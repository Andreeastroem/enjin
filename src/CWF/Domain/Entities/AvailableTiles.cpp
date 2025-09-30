#include "AvailableTiles.h"

namespace cwf::domain
{
    void AvailableTiles::addTile(AvailableTiles::Tile tile)
    {
        tiles.insert({tile.id, tile});
    }
    std::unordered_map<TileId, AvailableTiles::Tile> AvailableTiles::getTiles()
    {
        return tiles;
    }
    AvailableTiles::Tile *AvailableTiles::getTile(TileId tileId)
    {
        auto tileIndex = tiles.find(tileId);
        if (tileIndex != tiles.end())
        {
            auto tile = tiles.at(tileId);
            return &tile;
        }
        return nullptr;
    }
}