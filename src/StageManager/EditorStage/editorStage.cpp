#include "editorStage.h"

#include "clay_renderer/renderer.h"

#include "loaders/Loaders.h"

#include <iostream>

namespace
{
    struct gridSize
    {
        static constexpr u_int rows = 70;
        static constexpr u_int cols = 70;
        static constexpr u_int cellSize = 60;
    };

    // Example tile types
    enum TileTypes
    {
        EMPTY = 0,
        GRASS = 1,
        WATER = 2,
        SAND = 3,
        TREE = 4,
        MOUNTAIN = 5
    };
}

EditorStage::EditorStage(Clay_Context *clayContext_, Font *fonts_)
    : clayContext(clayContext_),
      fonts(fonts_),
      view((float)gridSize::cellSize)
{
    name = "Editor";
    ui::Init(uiState);

    initializeGame();
}

void EditorStage::onEnter()
{
    // Reset any transient UI when re-entering.
    ui::Init(uiState);
}

void EditorStage::initializeGame()
{
    try
    {
        loaders::LoadTilemapAndWriteWeights("island.pattern", "tinyWeights.json");

        pattern = loaders::LoadPatternAndConfigureRules("pattern_example.txt", rules);

        grid = std::make_unique<cwf::Grid>(gridSize::cols, gridSize::rows);

        // Get unique tile IDs from the rules system
        auto [charToId, idToChar] = pattern.tileMapping;
        possibleStates.clear();
        for (const auto &[_, id] : charToId)
        {
            possibleStates.push_back(id);
        }

        if (possibleStates.empty())
        {
            possibleStates = {GRASS, WATER, SAND, TREE};
        }

        grid->initialize(possibleStates, rules);

        visuals.clear();
        visuals[GRASS] = cwf::TileVisual{GREEN, nullptr, Rectangle{0, 0, 0, 0}};
        visuals[WATER] = cwf::TileVisual{BLUE, nullptr, Rectangle{0, 0, 0, 0}};
        visuals[SAND] = cwf::TileVisual{BEIGE, nullptr, Rectangle{0, 0, 0, 0}};
        visuals[TREE] = cwf::TileVisual{DARKGREEN, nullptr, Rectangle{0, 0, 0, 0}};
        visuals[MOUNTAIN] = cwf::TileVisual{DARKGRAY, nullptr, Rectangle{0, 0, 0, 0}};
        grid->setVisuals(visuals);

        view.initialize(*grid, 300);
        viewInitialized = true;

        initialized = true;
        initError.clear();
    }
    catch (const std::exception &e)
    {
        initialized = false;
        initError = e.what();
    }
    catch (...)
    {
        initialized = false;
        initError = "Unknown error";
    }
}

void EditorStage::update(float deltaTime)
{
    (void)deltaTime;

    if (!initialized)
    {
        if (IsKeyPressed(KEY_ESCAPE))
        {
            requestStageChange("Menu");
        }
        return;
    }

    if (!uiState.showSavePrompt && IsKeyPressed(KEY_ESCAPE))
    {
        requestStageChange("Menu");
        return;
    }

    // Debug: simulate game over
    if (!uiState.showSavePrompt && IsKeyPressed(KEY_K))
    {
        requestStageChange("GameOver");
        return;
    }

    // UI shortcuts and dialog input
    ui::HandleShortcuts(uiState);
    ui::HandleSaveDialogInput(uiState, *grid);

    if (isGenerating)
    {
        if (!grid->collapseStep())
        {
            isGenerating = false;
        }
    }

    if (!uiState.showSavePrompt)
    {
        if (viewInitialized)
        {
            view.update(*grid);
        }

        if (IsKeyPressed(KEY_SPACE))
        {
            isGenerating = !isGenerating;
        }

        if (IsKeyPressed(KEY_R))
        {
            grid = std::make_unique<cwf::Grid>(gridSize::cols, gridSize::rows);
            grid->initialize(possibleStates, rules);
            grid->setVisuals(visuals);
            isGenerating = false;

            view.initialize(*grid, 300);
            viewInitialized = true;
        }

        if (IsKeyPressed(KEY_M) && viewInitialized)
        {
            view.toggleMinimapVisible();
        }
        if (IsKeyPressed(KEY_N) && viewInitialized)
        {
            view.toggleMinimapSize(150, 300);
        }

        if (viewInitialized)
        {
            view.handleInput(*grid);
        }
    }
}

void EditorStage::render()
{
    if (!initialized)
    {
        DrawText("Game stage failed to initialize.", 30, 30, 20, RED);
        DrawText("Press ESC to return to menu.", 30, 60, 20, DARKGRAY);
        if (!initError.empty())
        {
            DrawText(initError.c_str(), 30, 90, 18, DARKGRAY);
        }
        return;
    }

    if (!grid)
    {
        return;
    }

    if (viewInitialized)
    {
        BeginMode2D(view.camera());
        grid->draw((float)gridSize::cellSize, 0.0f, 0.0f);
        EndMode2D();

        view.render(*grid);
    }

    // Draw UI (sidebar + save modal)
    Clay_Raylib_Render(ui::SideBar(clayContext, *grid, uiState), fonts);
}
