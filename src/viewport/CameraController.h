#pragma once

#include "raylib.h"
#include <cstddef>

namespace cwf
{
    class Grid;
}

namespace viewport
{
    class CameraController
    {
    public:
        explicit CameraController(float initialCellSize);

        void setCellSize(float s);
        float getCellSize() const;

        Camera2D &getCamera();
        const Camera2D &getCamera() const;

        // Handle input (WASD/arrows, RMB drag, wheel zoom) and clamp to grid bounds
        void update(const cwf::Grid &grid);

        // Optionally reset camera
        void reset();

    private:
        void clampToGrid(const cwf::Grid &grid);

    private:
        Camera2D camera{};
        float cellSize{30.0f};
    };
}
