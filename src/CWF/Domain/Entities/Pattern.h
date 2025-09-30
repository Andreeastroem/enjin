#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <array>
#include <optional>
#include "../ValueObjects/TileId.h"
#include "../ValueObjects/Direction.h"

namespace cwf::domain
{

    class Pattern
    {
    public:
        struct Connection
        {
            TileId toTile;
            float weight;

            Connection(TileId to, float w = 1.0f)
                : toTile(to), weight(w) {}
        };

        void addConnection(TileId from, Direction dir, TileId to, float weight = 1.0f);
        std::vector<Connection> getConnections(TileId from, Direction dir) const;
        bool canConnect(TileId from, Direction dir, TileId to) const;
        const std::unordered_map<TileId, std::array<std::vector<Connection>, 4>> &getAllConnections() const;

        void addTileMapping(char symbol, TileId id);
        std::unordered_map<TileId, char> getTileMappings() const;
        std::optional<TileId> getTileId(char symbol) const;
        std::optional<char> getSymbol(TileId id) const;

    private:
        std::unordered_map<TileId, std::array<std::vector<Connection>, 4>> connections_;
        std::unordered_map<char, TileId> char_to_id_;
        std::unordered_map<TileId, char> id_to_char_;
    };

} // namespace cwf::domain