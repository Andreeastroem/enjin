#pragma once

#include <vector>
#include <memory>
#include "../ValueObjects/TileId.h"

namespace cwf::domain
{

    class Tile
    {
    public:
        Tile();

        // Domain methods
        bool isCollapsed() const;
        size_t entropy() const;

        void collapse(TileId state);
        void addPossibleState(TileId state);
        void removePossibleState(TileId state);

        const std::vector<TileId> &possibleStates() const;
        TileId currentState() const;

    private:
        std::vector<TileId> possible_states_;
        TileId current_state_{0};
        bool collapsed_;
    };

} // namespace cwf::domain