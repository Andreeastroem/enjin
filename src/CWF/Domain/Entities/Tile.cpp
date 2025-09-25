#include "Tile.h"
#include <algorithm>

namespace cwf::domain
{

    Tile::Tile() : collapsed_(false) {}

    bool Tile::isCollapsed() const { return collapsed_; }

    size_t Tile::entropy() const { return possible_states_.size(); }

    void Tile::collapse(TileId state)
    {
        if (isCollapsed())
        {
            return;
        }
        current_state_ = state;
        possible_states_.clear();
        possible_states_.push_back(state);
        collapsed_ = true;
    }

    void Tile::addPossibleState(TileId state)
    {
        if (!isCollapsed())
        {
            possible_states_.push_back(state);
        }
    }

    void Tile::removePossibleState(TileId state)
    {
        if (isCollapsed())
        {
            return;
        }
        auto it = std::find_if(possible_states_.begin(), possible_states_.end(),
                               [&state](const TileId &s)
                               { return s == state; });
        if (it != possible_states_.end())
        {
            possible_states_.erase(it);
        }

        // Auto-collapse if only one state remains
        if (possible_states_.size() == 1)
        {
            collapse(possible_states_[0]);
        }
    }

    const std::vector<TileId> &Tile::possibleStates() const
    {
        return possible_states_;
    }

    TileId Tile::currentState() const
    {
        return current_state_;
    }

} // namespace cwf::domain