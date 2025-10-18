#include "viewport/Viewport.h"

namespace viewport
{
    Viewport::Viewport(float initialCellSize)
        : cameraCtrl(initialCellSize)
    {
    }

    void Viewport::initialize(const cwf::Grid &grid, int minimapMaxEdgePx)
    {
        minimap.initialize(grid, minimapMaxEdgePx);
    }

    void Viewport::update(const cwf::Grid &grid)
    {
        cameraCtrl.update(grid);
    }

    void Viewport::handleInput(const cwf::Grid &grid)
    {
        minimap.handleInput(grid, cameraCtrl.getCamera(), cameraCtrl.getCellSize());
    }

    void Viewport::render(const cwf::Grid &grid)
    {
        minimap.render(grid, cameraCtrl.getCellSize(), cameraCtrl.getCamera());
    }
}
