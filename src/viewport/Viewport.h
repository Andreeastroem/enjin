#pragma once

#include "raylib.h"

#include "viewport/CameraController.h"
#include "viewport/Minimap.h"

namespace cwf
{
    class Grid;
}

namespace viewport
{
    class Viewport
    {
    public:
        explicit Viewport(float initialCellSize);

        // Initialize minimap with desired max edge size
        void initialize(const cwf::Grid &grid, int minimapMaxEdgePx);

        // Per-frame updates
        void update(const cwf::Grid &grid);
        void handleInput(const cwf::Grid &grid);

        // Rendering
        void render(const cwf::Grid &grid);

        // Accessors
        Camera2D &camera() { return cameraCtrl.getCamera(); }
        const Camera2D &camera() const { return cameraCtrl.getCamera(); }
        float cellSize() const { return cameraCtrl.getCellSize(); }
        void setCellSize(float s) { cameraCtrl.setCellSize(s); }

        // Minimap controls passthrough
        void toggleMinimapVisible() { minimap.toggleVisible(); }
        void toggleMinimapSize(int smallPx, int largePx) { minimap.toggleSize(smallPx, largePx); }
        bool minimapVisible() const { return minimap.isVisible(); }

    private:
        CameraController cameraCtrl;
        Minimap minimap;
    };
}
