#pragma once

#include "raylib.h"
#include <cstddef>

namespace cwf
{
    class Grid;
}

namespace viewport
{
    class Minimap
    {
    public:
        Minimap();
        ~Minimap();

        // Initialize or resize minimap for a grid using desired max edge size
        void initialize(const cwf::Grid &grid, int maxEdgePixels);
        void release();

        // Render grid to texture and draw minimap with viewport rectangle
        void render(const cwf::Grid &grid, float cellSize, const Camera2D &camera);

        // Handle input for minimap interactions (e.g., click-to-pan)
        void handleInput(const cwf::Grid &grid, Camera2D &camera, float cellSize);

        // Optional customization
        void setMargin(int margin) { minimapMargin = margin; }
        void setVisible(bool v) { visible = v; }
        bool isVisible() const { return visible; }
        void toggleVisible() { visible = !visible; }
        void setMaxEdgePixels(int maxEdge) { desiredMaxEdge = maxEdge; }
        int getMaxEdgePixels() const { return desiredMaxEdge; }
        void toggleSize(int smallPx, int largePx) { desiredMaxEdge = (desiredMaxEdge == smallPx ? largePx : smallPx); }

    private:
        void ensureTexture(const cwf::Grid &grid);
        void drawViewportRect(const Camera2D &camera, float cellSize) const;

    private:
        RenderTexture2D minimapRT{};
        int mmCell{1};
        int minimapWidth{0};
        int minimapHeight{0};
        int minimapMargin{16};
        Color minimapBg{0, 0, 0, 140};
        Color minimapBorder{255, 255, 255, 160};
        Color minimapViewRect{255, 0, 0, 255};
        int mmX{0};
        int mmY{0};
        bool visible{true};
        int desiredMaxEdge{300};
        bool dragging{false};
        Vector2 dragOffsetWorld{0.0f, 0.0f};
    };
}
