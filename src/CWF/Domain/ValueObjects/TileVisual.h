#pragma once

#include "raylib.h"
#include "TileId.h"
#include <memory>

namespace cwf::domain
{
    struct TileVisual
    {
        Color color;
        std::shared_ptr<::Texture2D> texture;
        Rectangle sourceRect;

        TileVisual(::Color c,
                   std::shared_ptr<::Texture2D> tex = nullptr,
                   ::Rectangle rect = {})
            : color(c), sourceRect(rect), texture(tex) {}
    };
}