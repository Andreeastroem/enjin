#include "viewport/Minimap.h"
#include "CWF/grid.h"
#include <cmath>
#include <algorithm>

using namespace viewport;

Minimap::Minimap() {}

Minimap::~Minimap()
{
    release();
}

void Minimap::release()
{
    if (minimapRT.id != 0)
    {
        UnloadRenderTexture(minimapRT);
        minimapRT.id = 0;
    }
}

void Minimap::ensureTexture(const cwf::Grid &grid)
{
    int maxEdgePixels = desiredMaxEdge;
    int desiredCell = static_cast<int>(std::floorf(std::fminf(
        maxEdgePixels / static_cast<float>(grid.getWidth()),
        maxEdgePixels / static_cast<float>(grid.getHeight()))));
    if (desiredCell < 1)
        desiredCell = 1;

    int desiredW = static_cast<int>(grid.getWidth()) * desiredCell;
    int desiredH = static_cast<int>(grid.getHeight()) * desiredCell;

    bool needNew = (minimapRT.id == 0) || (desiredW != minimapWidth) || (desiredH != minimapHeight);
    if (needNew)
    {
        release();
        minimapRT = LoadRenderTexture(desiredW, desiredH);
        minimapWidth = desiredW;
        minimapHeight = desiredH;
        mmCell = desiredCell;
    }
}

void Minimap::initialize(const cwf::Grid &grid, int maxEdgePixels)
{
    desiredMaxEdge = maxEdgePixels;
    ensureTexture(grid);
}

void Minimap::drawViewportRect(const Camera2D &camera, float cellSize) const
{
    float mmScale = static_cast<float>(mmCell) / cellSize; // world->minimap scale
    float viewRectX = mmX + camera.target.x * mmScale;
    float viewRectY = mmY + camera.target.y * mmScale;
    float viewRectW = (GetScreenWidth() / camera.zoom) * mmScale;
    float viewRectH = (GetScreenHeight() / camera.zoom) * mmScale;

    if (viewRectX < mmX)
    {
        viewRectW -= (mmX - viewRectX);
        viewRectX = mmX;
    }
    if (viewRectY < mmY)
    {
        viewRectH -= (mmY - viewRectY);
        viewRectY = mmY;
    }
    if (viewRectX + viewRectW > mmX + minimapWidth)
        viewRectW = (mmX + minimapWidth) - viewRectX;
    if (viewRectY + viewRectH > mmY + minimapHeight)
        viewRectH = (mmY + minimapHeight) - viewRectY;
    if (viewRectW > 0 && viewRectH > 0)
        DrawRectangleLines(static_cast<int>(viewRectX), static_cast<int>(viewRectY), static_cast<int>(viewRectW), static_cast<int>(viewRectH), minimapViewRect);
}

void Minimap::render(const cwf::Grid &grid, float cellSize, const Camera2D &camera)
{
    if (!visible)
        return;
    // Keep texture sized appropriately
    int maxEdgeByScreen = std::min(desiredMaxEdge, std::min(GetScreenWidth(), GetScreenHeight()));
    desiredMaxEdge = maxEdgeByScreen; // clamp to screen if needed
    ensureTexture(grid);

    // Render to texture
    BeginTextureMode(minimapRT);
    ClearBackground({0, 0, 0, 0});
    grid.draw(static_cast<float>(mmCell), 0.0f, 0.0f);
    EndTextureMode();

    // Position in bottom-right
    mmX = GetScreenWidth() - minimapMargin - minimapWidth;
    mmY = GetScreenHeight() - minimapMargin - minimapHeight;

    // Panel and border
    DrawRectangle(mmX - 4, mmY - 4, minimapWidth + 8, minimapHeight + 8, minimapBg);
    DrawRectangleLines(mmX - 4, mmY - 4, minimapWidth + 8, minimapHeight + 8, minimapBorder);

    // Draw texture (flip Y)
    Rectangle src = {0.0f, 0.0f, static_cast<float>(minimapRT.texture.width), -static_cast<float>(minimapRT.texture.height)};
    Rectangle dst = {static_cast<float>(mmX), static_cast<float>(mmY), static_cast<float>(minimapWidth), static_cast<float>(minimapHeight)};
    DrawTexturePro(minimapRT.texture, src, dst, {0, 0}, 0.0f, WHITE);

    // Viewport rectangle
    drawViewportRect(camera, cellSize);
}

void Minimap::handleInput(const cwf::Grid &grid, Camera2D &camera, float cellSize)
{
    if (!visible)
        return;

    Vector2 m = GetMousePosition();
    bool inside = (m.x >= mmX && m.y >= mmY && m.x < mmX + minimapWidth && m.y < mmY + minimapHeight);
    float mmScale = static_cast<float>(mmCell) / cellSize;
    float viewW = GetScreenWidth() / camera.zoom;
    float viewH = GetScreenHeight() / camera.zoom;

    // Start dragging when pressing inside minimap
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && inside)
    {
        dragging = true;
        float currentCenterX = camera.target.x + viewW * 0.5f;
        float currentCenterY = camera.target.y + viewH * 0.5f;
        float cursorWorldX = (m.x - mmX) / mmScale;
        float cursorWorldY = (m.y - mmY) / mmScale;
        dragOffsetWorld = {currentCenterX - cursorWorldX, currentCenterY - cursorWorldY};
    }

    // Stop dragging when mouse released
    if (dragging && !IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        dragging = false;
    }

    // While dragging, update camera target to follow cursor (keeping offset)
    if (dragging)
    {
        float cursorWorldX = (m.x - mmX) / mmScale;
        float cursorWorldY = (m.y - mmY) / mmScale;
        float targetCenterX = cursorWorldX + dragOffsetWorld.x;
        float targetCenterY = cursorWorldY + dragOffsetWorld.y;
        camera.target.x = targetCenterX - viewW * 0.5f;
        camera.target.y = targetCenterY - viewH * 0.5f;

        // Clamp to bounds
        int gridPixelWidth = static_cast<int>(grid.getWidth() * cellSize);
        int gridPixelHeight = static_cast<int>(grid.getHeight() * cellSize);
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
}
