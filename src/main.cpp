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
#include "viewport/CameraController.h"
#include "viewport/Minimap.h"

// Save dialog state (global for access across UI and main loop)
static bool g_showSavePrompt = false;
static std::string g_saveFilename = "output.pattern";
static bool g_saveAttempted = false;
static bool g_saveSuccess = false;

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

cwf::Tile getTileFromCharacter(char character)
{
	switch (character)
	{
	case 'W':
		return cwf::Tile("Water", 2);
	case 'S':
		return cwf::Tile("Sand", 3);
	case 'T':
		return cwf::Tile("Tree", 4);
	case 'G':
	default:
		return cwf::Tile("Grass", 1);
		break;
	}
}

bool isValidCharacter(char character)
{
	char possibleValues[4] = {'G', 'S', 'W', 'T'};
	for (uint i = 0; i < 4; ++i)
	{
		if (character == possibleValues[i])
		{
			return true;
		}
	}

	return false;
}

std::vector<std::vector<cwf::Tile>> readTileMapFromFile(std::string filename)
{
	// TODO: read file and create tile with name and ID for each determined by the tileTypes
	std::vector<std::vector<cwf::Tile>> tilemap;
	std::ifstream s(filename);
	std::string row;

	if (!s.is_open())
	{
		return tilemap;
	}

	while (std::getline(s, row))
	{
		std::vector<cwf::Tile> rowTiles;
		for (char tileCharacter : row)
		{
			if (isValidCharacter(tileCharacter))
			{
				cwf::Tile t = getTileFromCharacter(tileCharacter);
				rowTiles.push_back(t);
			}
		}
		tilemap.push_back(rowTiles);
	}

	s.close();
	return tilemap;
}

void RenderText(Clay_String text)
{
	CLAY_AUTO_ID({.layout = {.padding = CLAY_PADDING_ALL(16)}})
	{
		CLAY_TEXT(text, CLAY_TEXT_CONFIG({.fontId = 0,
										  .fontSize = 16,
										  .textColor = {255, 255, 255, 255}}));
	}
}

// Forward declare the Save Modal (declares UI and handles interactions)
static void SaveModal(cwf::Grid &grid)
{
	if (!g_showSavePrompt)
		return;

	// Darken background overlay
	CLAY(CLAY_ID("SaveOverlay"), {.backgroundColor = {0, 0, 0, 128},
								  .floating = {
									  .attachTo = CLAY_ATTACH_TO_ROOT,
									  .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
									  .zIndex = 1000,
									  .attachPoints = {CLAY_ATTACH_POINT_CENTER_CENTER, CLAY_ATTACH_POINT_CENTER_CENTER},
									  .offset = {0, 0},
								  },
								  .layout = {.sizing = {
												 .width = CLAY_SIZING_PERCENT(1.0f),
												 .height = CLAY_SIZING_PERCENT(1.0f),
											 }}})
	{
		// Centered dialog panel
		CLAY(CLAY_ID("SaveDialogPanel"), {.backgroundColor = {35, 35, 35, 255},
										  .cornerRadius = CLAY_CORNER_RADIUS(8),
										  .floating = {
											  .attachTo = CLAY_ATTACH_TO_ROOT,
											  .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
											  .zIndex = 1001,
											  .attachPoints = {CLAY_ATTACH_POINT_CENTER_CENTER, CLAY_ATTACH_POINT_CENTER_CENTER},
											  .offset = {0, 0},
										  },
										  .layout = {
											  .padding = CLAY_PADDING_ALL(16),
											  .sizing = {
												  .width = CLAY_SIZING_FIXED(540),
												  .height = CLAY_SIZING_FIXED(220),
											  },
											  .layoutDirection = CLAY_TOP_TO_BOTTOM,
											  .childGap = 12,
											  .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_TOP},
										  }})
		{
			// Title
			RenderText(CLAY_STRING("Save As"));

			// Filename input label
			CLAY_AUTO_ID({.layout = {.sizing = {.width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_FIT(0, 9999)}}})
			{
				CLAY_TEXT(CLAY_STRING("Filename"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 18, .textColor = {230, 230, 230, 255}}));
			}

			// Filename input box (display only; input handled in main loop)
			CLAY(CLAY_ID("FilenameBox"), {.backgroundColor = {55, 55, 55, 255},
										  .border = {.color = {200, 200, 200, 255}, .width = CLAY_BORDER_ALL(1)},
										  .cornerRadius = CLAY_CORNER_RADIUS(4),
										  .layout = {
											  .padding = CLAY_PADDING_ALL(10),
											  .sizing = {
												  .width = CLAY_SIZING_GROW(1),
												  .height = CLAY_SIZING_FIXED(44),
											  },
											  .childAlignment = {CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER},
										  }})
			{
				// Show current filename
				// Convert std::string to Clay_String for rendering
				Clay_String dyn = {.isStaticallyAllocated = false, .length = (int)g_saveFilename.size(), .chars = g_saveFilename.c_str()};
				CLAY_TEXT(dyn, CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 20, .textColor = {255, 255, 255, 255}}));
			}

			// Action buttons row
			CLAY(CLAY_ID("ButtonsRow"), {.layout = {
											 .sizing = {.width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_FIT(0, 9999)},
											 .layoutDirection = CLAY_LEFT_TO_RIGHT,
											 .childAlignment = {CLAY_ALIGN_X_RIGHT, CLAY_ALIGN_Y_TOP},
											 .childGap = 12,
										 }})
			{
				// Cancel
				CLAY(CLAY_ID("CancelButton"), {.backgroundColor = {90, 90, 90, 255},
											   .cornerRadius = CLAY_CORNER_RADIUS(4),
											   .layout = {.padding = CLAY_PADDING_ALL(10), .sizing = {.width = CLAY_SIZING_FIT(0, 9999), .height = CLAY_SIZING_FIT(0, 9999)}}})
				{
					CLAY_TEXT(CLAY_STRING("Cancel"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 18, .textColor = {255, 255, 255, 255}}));
					if (Clay_Hovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						g_showSavePrompt = false;
						g_saveAttempted = false;
					}
				}

				// Save
				CLAY(CLAY_ID("SaveButton"), {.backgroundColor = {0, 120, 215, 255},
											 .cornerRadius = CLAY_CORNER_RADIUS(4),
											 .layout = {.padding = CLAY_PADDING_ALL(10), .sizing = {.width = CLAY_SIZING_FIT(0, 9999), .height = CLAY_SIZING_FIT(0, 9999)}}})
				{
					CLAY_TEXT(CLAY_STRING("Save"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 18, .textColor = {255, 255, 255, 255}}));
					if (Clay_Hovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						g_saveSuccess = grid.saveToFile(g_saveFilename);
						g_saveAttempted = true;
						if (g_saveSuccess)
						{
							g_showSavePrompt = false;
						}
					}
				}
			}

			// Optional status text if attempted
			if (g_saveAttempted)
			{
				CLAY_AUTO_ID({.layout = {.sizing = {.width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_FIT(0, 9999)}}})
				{
					if (g_saveSuccess)
					{
						CLAY_TEXT(CLAY_STRING("Saved."), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 16, .textColor = {150, 255, 150, 255}}));
					}
					else
					{
						CLAY_TEXT(CLAY_STRING("Failed to save."), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 16, .textColor = {255, 150, 150, 255}}));
					}
				}
			}
		}
	}
}

Clay_RenderCommandArray SideBar(Clay_Context *context, cwf::Grid &grid)
{
	Clay_BeginLayout();

	Clay_Sizing layoutExpand = {
		.width = CLAY_SIZING_PERCENT(0.20f),
		.height = CLAY_SIZING_GROW(1)};

	Clay_Color backgroundColor = {90, 90, 90, 200};

	CLAY(CLAY_ID("root"), {.layout = {
							   .sizing = {
								   .width = CLAY_SIZING_GROW(1),
								   .height = CLAY_SIZING_GROW(1),
							   }}})
	{

		CLAY(CLAY_ID("Container"), {.backgroundColor = backgroundColor,
									.layout = {
										.layoutDirection = CLAY_TOP_TO_BOTTOM,
										.sizing = layoutExpand,
										.childGap = 16,
									}})
		{
			RenderText(CLAY_STRING("Space: Start/Pause Generation"));
			RenderText(CLAY_STRING("R: Reset Grid"));
			RenderText(CLAY_STRING("Arrows/WASD or RMB drag: Pan"));
			RenderText(CLAY_STRING("Mouse wheel: Zoom"));
			RenderText(CLAY_STRING("M: Toggle minimap visibility"));
			RenderText(CLAY_STRING("N: Toggle minimap size"));
		}
	}
	// Save modal overlay (placed after to ensure it renders on top)
	SaveModal(grid);

	Clay_RenderCommandArray renderCommands = Clay_EndLayout();
	return renderCommands;
}

int main()
{
	// Clay setup
	Clay_Context *clayContext = setupClay();

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(windowSize::width, windowSize::height, "Enjin");
	SearchAndSetResourceDir("resources");

	Font fonts[1];
	// NOTE: After SearchAndSetResourceDir("resources") the working directory is the resources folder.
	// Load the font relative to the new CWD (no leading "resources/") so it actually loads.
	fonts[0] = LoadFontEx("AldotheApache.ttf", 48, 0, 400);

	auto tilemap = readTileMapFromFile("island.pattern");
	auto tileWeights = cwf::TileWeights();
	tileWeights.calculateTileWeights(tilemap);
	tileWeights.writeTileWeightsToFile("tinyWeights.json");

	// Load pattern from file
	cwf::TileRules rules;
	cwf::Pattern pattern;
	try
	{
		pattern = cwf::PatternLoader::loadPatternFromFile("pattern_example.txt");
		rules.addTileMapping(pattern.tileMapping);
		rules.learnPattern(pattern.charPattern);
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

	viewport::CameraController cameraCtrl(gridSize::cellSize);
	viewport::Minimap minimap;
	minimap.initialize(grid, 300);

	Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);
	Clay_SetDebugModeEnabled(false);

	while (!WindowShouldClose())
	{
		Vector2 mp = GetMousePosition();
		Clay_SetPointerState(Clay_Vector2{mp.x, mp.y}, IsMouseButtonDown(MOUSE_LEFT_BUTTON));
		if (IsWindowResized())
		{
			Clay_SetLayoutDimensions(Clay_Dimensions{(float)GetScreenWidth(), (float)GetScreenHeight()});
		}

		// Ctrl+S to open Save dialog
		bool ctrlDown = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
		if (!g_showSavePrompt && ctrlDown && IsKeyPressed(KEY_S))
		{
			g_showSavePrompt = true;
			g_saveAttempted = false;
		}

		// Handle text input for save dialog when open
		if (g_showSavePrompt)
		{
			// Enter to save
			if (IsKeyPressed(KEY_ENTER))
			{
				g_saveSuccess = grid.saveToFile(g_saveFilename);
				g_saveAttempted = true;
				if (g_saveSuccess)
				{
					g_showSavePrompt = false;
				}
			}
			// Escape to cancel
			if (IsKeyPressed(KEY_ESCAPE))
			{
				g_showSavePrompt = false;
				g_saveAttempted = false;
			}
			// Backspace support
			if (IsKeyPressed(KEY_BACKSPACE))
			{
				if (!g_saveFilename.empty())
				{
					g_saveFilename.pop_back();
				}
			}
			// Character input (basic ASCII)
			int key = 0;
			while ((key = GetCharPressed()) != 0)
			{
				if (key >= 32 && key < 127)
				{
					char c = (char)key;
					// Restrict to a safe set of filename characters
					if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.')
					{
						g_saveFilename.push_back(c);
					}
				}
			}
		}

		if (isGenerating)
		{
			// Perform one step of the wave function collapse
			if (!grid.collapseStep())
			{
				isGenerating = false; // Stop when complete
			}
		}

		if (!g_showSavePrompt)
		{

			// Update camera via controller
			cameraCtrl.update(grid);
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
				minimap.toggleVisible();
			}
			if (IsKeyPressed(KEY_N))
			{
				minimap.toggleSize(150, 300);
			}
			// Handle minimap click-to-pan
			minimap.handleInput(grid, cameraCtrl.getCamera(), gridSize::cellSize);
		}

		// Drawing
		BeginDrawing();
		ClearBackground(RAYWHITE);

		// Draw the grid in camera space
		BeginMode2D(cameraCtrl.getCamera());
		grid.draw(gridSize::cellSize, 0.0f, 0.0f);
		EndMode2D();

		// Minimap rendering
		minimap.render(grid, gridSize::cellSize, cameraCtrl.getCamera());

		// Draw UI (sidebar + save modal)
		Clay_Raylib_Render(SideBar(clayContext, grid), fonts);

		EndDrawing();
	}

	// Cleanup
	minimap.release();
	Clay_Raylib_Close();
	return 0;
}
