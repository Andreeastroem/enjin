#pragma once

#include <unordered_map>
#include <memory>
#include "../../Domain/ValueObjects/TileId.h"
#include "../../Domain/ValueObjects/TileVisual.h"

namespace cwf::application
{
    class TileVisualService
    {
    public:
        static TileVisualService &getInstance()
        {
            static TileVisualService instance;
            return instance;
        }

        void setVisual(domain::TileId id, domain::TileVisual visual)
        {
            visuals_.insert_or_assign(id, std::move(visual));
        }

        const domain::TileVisual *getVisual(domain::TileId id) const
        {
            auto it = visuals_.find(id);
            if (it != visuals_.end())
            {
                return &it->second;
            }
            return nullptr;
        }

        void clearVisuals()
        {
            visuals_.clear();
        }

    private:
        TileVisualService() = default;
        std::unordered_map<domain::TileId, domain::TileVisual> visuals_;
    };
}