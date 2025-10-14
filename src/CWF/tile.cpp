#include "tile.h"

namespace cwf
{
    Tile::Tile() : currentState(-1), collapsed(false)
    {
    }

    Tile::Tile(std::string name, TileId id)
    {
        Tile();
        this->name = name;
        this->id = id;
    }

    void Tile::collapse(TileId state)
    {
        if (std::find(possibleStates.begin(), possibleStates.end(), state) == possibleStates.end())
        {
            throw std::runtime_error("Attempting to collapse to invalid state");
        }

        possibleStates.clear();
        possibleStates.push_back(state);
        currentState = state;
        collapsed = true;
    }

    void Tile::addPossibleState(TileId state)
    {
        if (!collapsed && std::find(possibleStates.begin(), possibleStates.end(), state) == possibleStates.end())
        {
            possibleStates.push_back(state);
        }
    }

    void Tile::removePossibleState(TileId state)
    {
        if (collapsed)
            return;

        auto it = std::find(possibleStates.begin(), possibleStates.end(), state);
        if (it != possibleStates.end())
        {
            possibleStates.erase(it);
        }

        // If only one state remains, collapse to it
        if (possibleStates.size() == 1)
        {
            collapse(possibleStates[0]);
        }
    }

    const Tile::TileId Tile::getCurrectState()
    {
        return id;
    }
}