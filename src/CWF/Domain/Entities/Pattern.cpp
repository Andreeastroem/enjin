#include "Pattern.h"
#include <algorithm>

namespace cwf::domain
{

    void Pattern::addConnection(TileId from, Direction dir, TileId to, float weight)
    {
        connections_[from][static_cast<size_t>(dir)].emplace_back(to, weight);
    }

    std::vector<Pattern::Connection> Pattern::getConnections(TileId from, Direction dir) const
    {
        auto it = connections_.find(from);
        if (it == connections_.end())
        {
            return {};
        }
        return it->second[static_cast<size_t>(dir)];
    }

    bool Pattern::canConnect(TileId from, Direction dir, TileId to) const
    {
        const auto &connections = getConnections(from, dir);
        return std::any_of(connections.begin(), connections.end(),
                           [&to](const Connection &conn)
                           { return conn.toTile == to; });
    }

    const std::unordered_map<TileId, std::array<std::vector<Pattern::Connection>, 4>> &Pattern::getAllConnections() const
    {
        return connections_;
    }

    void Pattern::addTileMapping(char symbol, TileId id)
    {
        char_to_id_[symbol] = id;
        id_to_char_[id] = symbol;
    }

    std::unordered_map<TileId, char> Pattern::getTileMappings() const
    {
        return id_to_char_;
    }

    std::optional<TileId> Pattern::getTileId(char symbol) const
    {
        auto it = char_to_id_.find(symbol);
        if (it != char_to_id_.end())
        {
            return it->second;
        }
        return std::nullopt;
    }

    std::optional<char> Pattern::getSymbol(TileId id) const
    {
        auto it = id_to_char_.find(id);
        if (it != id_to_char_.end())
        {
            return it->second;
        }
        return std::nullopt;
    }

} // namespace cwf::domain