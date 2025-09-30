#pragma once

#include <string>
#include "CWF/Domain/ValueObjects/TileId.h"
#include <unordered_map>

namespace cwf::domain
{

    class AvailableTiles
    {
    public:
        struct Tile
        {
            TileId id;
            std::string name;
            std::string character;

            Tile(TileId tileId, std::string tileName, std::string tileCharacter) : id(tileId), name(tileName), character(tileCharacter) {}
            bool operator<(const Tile &other) const { id < other.id; };
            bool operator==(const Tile &other) const { id == other.id; };
            bool operator!=(const Tile &other) const { id != other.id; };
        };

        void addTile(Tile tile);
        std::unordered_map<TileId, Tile> getTiles();
        Tile *getTile(TileId tileId);

    private:
        std::unordered_map<TileId, Tile> tiles;
    };
}