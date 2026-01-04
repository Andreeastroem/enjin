#pragma once

#include "../stage.h"

#include "raylib.h"

#include "clay.h"

#include "CWF/grid.h"
#include "CWF/TileUtils.h"
#include "CWF/tile_rules.h"
#include "viewport/Viewport.h"
#include "ui/Ui.h"

#include <memory>
#include <string>
#include <vector>

class EditorStage : public Stage
{
public:
    EditorStage(Clay_Context *clayContext, Font *fonts);

    void update(float deltaTime) override;
    void render() override;

protected:
    void onEnter() override;

private:
    void initializeGame();

private:
    Clay_Context *clayContext;
    Font *fonts;

    ui::UiState uiState;

    bool initialized = false;
    std::string initError;

    bool isGenerating = false;

    cwf::TileRules rules;
    cwf::Pattern pattern;

    std::unique_ptr<cwf::Grid> grid;
    cwf::TileVisuals visuals;
    std::vector<cwf::Tile::TileId> possibleStates;

    viewport::Viewport view;
    bool viewInitialized = false;
};
