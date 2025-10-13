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

struct windowSize
{
	static const u_int height = 1080;
	static const u_int width = 1920;
};

struct gridSize
{
	static const u_int rows = 70;
	static const u_int cols = 70;
};

void handleClayErrors(Clay_ErrorData errorData)
{
	printf("%s", errorData.errorText.chars);
}

void setupClay()
{
	uint64_t totalMemorySize = Clay_MinMemorySize();
	Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, (char *)malloc(totalMemorySize));
	Clay_Initialize(clayMemory, Clay_Dimensions{windowSize::width, windowSize::height}, Clay_ErrorHandler{handleClayErrors});
}

// Example tile types
enum TileTypes
{
	EMPTY = 0,
	GRASS = 1,
	WATER = 2,
	SAND = 3,
	TREE = 4
};

cwf::Tile getTileFromCharacter(char character)
{
	switch (character)
	{
	case 'G':
		return cwf::Tile("Grass", 1);
	case 'W':
		return cwf::Tile("Water", 2);
	case 'S':
		return cwf::Tile("Sand", 3);
	case 'T':
		return cwf::Tile("Tree", 4);
	default:
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

int main()
{
	// Clay setup
	setupClay();

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(windowSize::width, windowSize::height, "Wave Function Collapse Example");
	SearchAndSetResourceDir("resources");

	auto tilemap = readTileMapFromFile("pattern.pattern");
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
	grid.setVisuals(visuals);

	bool isGenerating = false;
	float cellSize = 30.0f; // Size of each tile in main view (world units are pixels)

	// Camera2D setup for panning/zooming the main viewport
	Camera2D camera = {0};
	camera.target = {0.0f, 0.0f}; // top-left world coordinate visible at screen (with offset {0,0})
	camera.offset = {0.0f, 0.0f}; // no centering; world (0,0) draws at screen (0,0)
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	// Minimap setup
	const int minimapMaxSize = 300; // maximum width/height in pixels for minimap
	// Compute minimap cell size to fit grid within minimapMaxSize box
	int gridPixelWidth = static_cast<int>(grid.getWidth() * cellSize);
	int gridPixelHeight = static_cast<int>(grid.getHeight() * cellSize);
	float mmCellF = std::floorf(std::fminf(
		minimapMaxSize / static_cast<float>(grid.getWidth()),
		minimapMaxSize / static_cast<float>(grid.getHeight())));
	if (mmCellF < 1.0f)
		mmCellF = 1.0f;
	int mmCell = static_cast<int>(mmCellF);
	int minimapWidth = static_cast<int>(grid.getWidth()) * mmCell;
	int minimapHeight = static_cast<int>(grid.getHeight()) * mmCell;
	RenderTexture2D minimapRT = LoadRenderTexture(minimapWidth, minimapHeight);
	const int minimapMargin = 16; // margin from bottom-right
	const Color minimapBg = Color{0, 0, 0, 140};
	const Color minimapBorder = Color{255, 255, 255, 160};
	const Color minimapViewRect = RED;

	while (!WindowShouldClose())
	{
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

		// Camera input: panning with arrow keys or right-mouse drag
		float panStep = 15.0f / camera.zoom; // screen-consistent pan speed
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

		// Zoom with mouse wheel (focus-in-place not implemented; simple zoom)
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

		// Clamp camera to grid bounds
		int winW = GetScreenWidth();
		int winH = GetScreenHeight();
		float viewW = winW / camera.zoom;
		float viewH = winH / camera.zoom;
		float maxX = std::max(0.0f, gridPixelWidth - viewW);
		float maxY = std::max(0.0f, gridPixelHeight - viewH);
		if (camera.target.x < 0.0f)
			camera.target.x = 0.0f;
		if (camera.target.y < 0.0f)
			camera.target.y = 0.0f;
		if (camera.target.x > maxX)
			camera.target.x = maxX;
		if (camera.target.y > maxY)
			camera.target.y = maxY;

		if (isGenerating)
		{
			// Perform one step of the wave function collapse
			if (!grid.collapseStep())
			{
				isGenerating = false; // Stop when complete
			}
		}

		// Drawing
		BeginDrawing();
		ClearBackground(RAYWHITE);

		// Draw the grid in camera space
		BeginMode2D(camera);
		grid.draw(cellSize, 0.0f, 0.0f);
		EndMode2D();

		// Minimap: render to texture
		BeginTextureMode(minimapRT);
		ClearBackground({0, 0, 0, 0});
		grid.draw(static_cast<float>(mmCell), 0.0f, 0.0f);
		EndTextureMode();

		// Draw minimap background panel and texture in bottom-right
		int mmX = GetScreenWidth() - minimapMargin - minimapWidth;
		int mmY = GetScreenHeight() - minimapMargin - minimapHeight;
		DrawRectangle(mmX - 4, mmY - 4, minimapWidth + 8, minimapHeight + 8, minimapBg);
		DrawRectangleLines(mmX - 4, mmY - 4, minimapWidth + 8, minimapHeight + 8, minimapBorder);
		Rectangle src = {0.0f, 0.0f, static_cast<float>(minimapRT.texture.width), -static_cast<float>(minimapRT.texture.height)}; // flip Y
		Rectangle dst = {static_cast<float>(mmX), static_cast<float>(mmY), static_cast<float>(minimapWidth), static_cast<float>(minimapHeight)};
		DrawTexturePro(minimapRT.texture, src, dst, {0, 0}, 0.0f, WHITE);

		// Draw viewport rectangle on minimap
		float mmScale = static_cast<float>(mmCell) / cellSize; // world->minimap scale
		float viewRectX = mmX + camera.target.x * mmScale;
		float viewRectY = mmY + camera.target.y * mmScale;
		float viewRectW = (GetScreenWidth() / camera.zoom) * mmScale;
		float viewRectH = (GetScreenHeight() / camera.zoom) * mmScale;
		// Clamp viewport rect within minimap bounds
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

		// Draw instructions
		DrawText("Space: Start/Pause Generation", 10, 10, 20, DARKGRAY);
		DrawText("R: Reset Grid", 10, 40, 20, DARKGRAY);
		DrawText("Arrows/WASD or RMB drag: Pan", 10, 70, 20, DARKGRAY);
		DrawText("Mouse wheel: Zoom", 10, 100, 20, DARKGRAY);

		EndDrawing();
	}

	// Cleanup
	UnloadRenderTexture(minimapRT);
	CloseWindow();
	return 0;
}
