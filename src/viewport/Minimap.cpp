#include "viewport/Minimap.h"
#include "CWF/grid.h"
#include <cmath>

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

void Minimap::ensureTexture(const cwf::Grid &grid, int maxEdgePixels)
{
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
    ensureTexture(grid, maxEdgePixels);
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
    // Keep texture sized appropriately
    ensureTexture(grid, std::min(300, std::min(GetScreenWidth(), GetScreenHeight())));

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
