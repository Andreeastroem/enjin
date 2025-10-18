#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay_renderer/renderer.h"
#include "CWF/tile_rules.h"
#include "CWF/grid.h"
#include "CWF/pattern_loader.h"

#include "raylib.h"
#include "StageManager/stageManager.h"

#include "CWF/tile_restrictions.h"

#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#include <fstream>
#include "CWF/tile_weights.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include "viewport/Viewport.h"
#include "ui/Ui.h"
#include "loaders/Loaders.h"

// UI State
static ui::UiState g_uiState;

struct windowSize
{
	static const u_int height = 1080;
	static const u_int width = 1920;
};

struct gridSize
{
	static const u_int rows = 70;
	static const u_int cols = 70;
	static const u_int cellSize = 60;
};

void handleClayErrors(Clay_ErrorData errorData)
{
	printf("%s", errorData.errorText.chars);
}

Clay_Context *setupClay()
{
	uint64_t totalMemorySize = Clay_MinMemorySize();
	Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, (char *)malloc(totalMemorySize));
	return Clay_Initialize(clayMemory, Clay_Dimensions{windowSize::width, windowSize::height}, Clay_ErrorHandler{handleClayErrors});
}

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

// Removed local UI helpers: now using ui:: functions

int main()
{
	// Clay setup
	Clay_Context *clayContext = setupClay();

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(windowSize::width, windowSize::height, "Enjin");
	SetExitKey(KEY_NULL);
	SearchAndSetResourceDir("resources");

	Font fonts[1];
	// NOTE: After SearchAndSetResourceDir("resources") the working directory is the resources folder.
	// Load the font relative to the new CWD (no leading "resources/") so it actually loads.
	fonts[0] = LoadFontEx("AldotheApache.ttf", 48, 0, 400);

	loaders::LoadTilemapAndWriteWeights("island.pattern", "tinyWeights.json");

	// Load pattern from file
	cwf::TileRules rules;
	cwf::Pattern pattern;
	try
	{
		pattern = loaders::LoadPatternAndConfigureRules("pattern_example.txt", rules);
	}
	catch (const std::exception &e)
	{
		std::cout << "something went wrong" << std::endl;
		return 1;
	}

	// Create WFC grid
	cwf::Grid grid(gridSize::cols, gridSize::rows);

	// Get unique tile IDs from the rules system
	auto [charToId, idToChar] = pattern.tileMapping;
	std::vector<cwf::Tile::TileId> possibleStates;
	for (const auto &[_, id] : charToId)
	{
		possibleStates.push_back(id);
	}

	if (possibleStates.empty())
	{
		possibleStates = {GRASS, WATER, SAND, TREE};
	}

	grid.initialize(possibleStates, rules); // Set up visuals for the tiles
	cwf::TileVisuals visuals;
	visuals[GRASS] = cwf::TileVisual{GREEN, nullptr, Rectangle{0, 0, 0, 0}};
	visuals[WATER] = cwf::TileVisual{BLUE, nullptr, Rectangle{0, 0, 0, 0}};
	visuals[SAND] = cwf::TileVisual{BEIGE, nullptr, Rectangle{0, 0, 0, 0}};
	visuals[TREE] = cwf::TileVisual{DARKGREEN, nullptr, Rectangle{0, 0, 0, 0}};
	visuals[MOUNTAIN] = cwf::TileVisual{DARKGRAY, nullptr, Rectangle{0, 0, 0, 0}};
	grid.setVisuals(visuals);

	bool isGenerating = false;

	viewport::Viewport view(gridSize::cellSize);
	view.initialize(grid, 300);

	Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);
	Clay_SetDebugModeEnabled(false);

	// Initialize UI state
	ui::Init(g_uiState);

	while (!WindowShouldClose())
	{
		Vector2 mp = GetMousePosition();
		Clay_SetPointerState(Clay_Vector2{mp.x, mp.y}, IsMouseButtonDown(MOUSE_LEFT_BUTTON));
		if (IsWindowResized())
		{
			Clay_SetLayoutDimensions(Clay_Dimensions{(float)GetScreenWidth(), (float)GetScreenHeight()});
		}

		// UI shortcuts and dialog input
		ui::HandleShortcuts(g_uiState);
		ui::HandleSaveDialogInput(g_uiState, grid);

		if (isGenerating)
		{
			// Perform one step of the wave function collapse
			if (!grid.collapseStep())
			{
				isGenerating = false; // Stop when complete
			}
		}

		if (!g_uiState.showSavePrompt)
		{
			// Update camera via facade
			view.update(grid);
			// Update
			if (IsKeyPressed(KEY_SPACE))
			{
				isGenerating = !isGenerating;
			}

			if (IsKeyPressed(KEY_R))
			{
				// Reset grid
				grid = cwf::Grid(gridSize::cols, gridSize::rows);
				grid.initialize(possibleStates, rules);
				grid.setVisuals(visuals);
				isGenerating = false;
			}
			// Minimap controls: toggle visibility (M), toggle size (N small/large)
			if (IsKeyPressed(KEY_M))
			{
				view.toggleMinimapVisible();
			}
			if (IsKeyPressed(KEY_N))
			{
				view.toggleMinimapSize(150, 300);
			}
			// Handle minimap click-to-pan
			view.handleInput(grid);
		}

		// Drawing
		BeginDrawing();
		ClearBackground(RAYWHITE);

		// Draw the grid in camera space
		BeginMode2D(view.camera());
		grid.draw(gridSize::cellSize, 0.0f, 0.0f);
		EndMode2D();

		// Minimap rendering
		view.render(grid);

		// Draw UI (sidebar + save modal)
		Clay_Raylib_Render(ui::SideBar(clayContext, grid, g_uiState), fonts);

		EndDrawing();
	}

	// Cleanup
	Clay_Raylib_Close();
	return 0;
}
