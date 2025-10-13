#include "viewport/CameraController.h"
#include "CWF/grid.h"
#include <algorithm>

using namespace viewport;

CameraController::CameraController(float initialCellSize)
{
    cellSize = initialCellSize;
    camera.target = {0.0f, 0.0f};
    camera.offset = {0.0f, 0.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void CameraController::setCellSize(float s)
{
    cellSize = s;
}

float CameraController::getCellSize() const
{
    return cellSize;
}

Camera2D &CameraController::getCamera()
{
    return camera;
}

const Camera2D &CameraController::getCamera() const
{
    return camera;
}

void CameraController::reset()
{
    camera.target = {0.0f, 0.0f};
    camera.zoom = 1.0f;
}

void CameraController::clampToGrid(const cwf::Grid &grid)
{
    int gridPixelWidth = static_cast<int>(grid.getWidth() * cellSize);
    int gridPixelHeight = static_cast<int>(grid.getHeight() * cellSize);
    int winW = GetScreenWidth();
    int winH = GetScreenHeight();
    float viewW = winW / camera.zoom;
    float viewH = winH / camera.zoom;
    float maxX = std::max(0.0f, static_cast<float>(gridPixelWidth) - viewW);
    float maxY = std::max(0.0f, static_cast<float>(gridPixelHeight) - viewH);
    if (camera.target.x < 0.0f)
        camera.target.x = 0.0f;
    if (camera.target.y < 0.0f)
        camera.target.y = 0.0f;
    if (camera.target.x > maxX)
        camera.target.x = maxX;
    if (camera.target.y > maxY)
        camera.target.y = maxY;
}

void CameraController::update(const cwf::Grid &grid)
{
    float panStep = 15.0f / camera.zoom;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        camera.target.x += panStep;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        camera.target.x -= panStep;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        camera.target.y += panStep;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        camera.target.y -= panStep;

    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
    {
        Vector2 delta = GetMouseDelta();
        camera.target.x -= delta.x / camera.zoom;
        camera.target.y -= delta.y / camera.zoom;
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f)
    {
        float zoomFactor = 1.0f + wheel * 0.1f;
        camera.zoom *= zoomFactor;
        if (camera.zoom < 0.25f)
            camera.zoom = 0.25f;
        if (camera.zoom > 5.0f)
            camera.zoom = 5.0f;
    }

    // clampToGrid(grid);
}
