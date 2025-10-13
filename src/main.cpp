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

	viewport::CameraController cameraCtrl(cellSize);
	viewport::Minimap minimap;
	minimap.initialize(grid, 300);

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

		// Update camera via controller
		cameraCtrl.update(grid);

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
		BeginMode2D(cameraCtrl.getCamera());
		grid.draw(cellSize, 0.0f, 0.0f);
		EndMode2D();

		// Minimap rendering
		minimap.render(grid, cellSize, cameraCtrl.getCamera());

		// Draw instructions
		DrawText("Space: Start/Pause Generation", 10, 10, 20, DARKGRAY);
		DrawText("R: Reset Grid", 10, 40, 20, DARKGRAY);
		DrawText("Arrows/WASD or RMB drag: Pan", 10, 70, 20, DARKGRAY);
		DrawText("Mouse wheel: Zoom", 10, 100, 20, DARKGRAY);

		EndDrawing();
	}

	// Cleanup
	minimap.release();
	CloseWindow();
	return 0;
}
