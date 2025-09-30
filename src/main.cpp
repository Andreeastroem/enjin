#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay_renderer/renderer.h"
#include "CWF/tile_rules.h"
#include "CWF/grid.h"
#include "CWF/pattern_loader.h"

#include "raylib.h"
#include "StageManager/stageManager.h"

#include "resource_dir.h" // utility header for SearchAndSetResourceDir

void handleClayErrors(Clay_ErrorData errorData)
{
	printf("%s", errorData.errorText.chars);
}

void setupClay()
{
	uint64_t totalMemorySize = Clay_MinMemorySize();
	Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, (char *)malloc(totalMemorySize));
	Clay_Initialize(clayMemory, Clay_Dimensions{1024, 768}, Clay_ErrorHandler{handleClayErrors});
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

void setupWFCRules(cwf::TileRules &rules)
{
	// GRASS can connect to GRASS and SAND
	rules.addConnection(GRASS, cwf::Direction::NORTH, GRASS);
	rules.addConnection(GRASS, cwf::Direction::EAST, GRASS);
	rules.addConnection(GRASS, cwf::Direction::SOUTH, GRASS);
	rules.addConnection(GRASS, cwf::Direction::WEST, GRASS);

	rules.addConnection(GRASS, cwf::Direction::NORTH, SAND);
	rules.addConnection(GRASS, cwf::Direction::EAST, SAND);
	rules.addConnection(GRASS, cwf::Direction::SOUTH, SAND);
	rules.addConnection(GRASS, cwf::Direction::WEST, SAND);

	rules.addConnection(GRASS, cwf::Direction::NORTH, TREE);
	rules.addConnection(GRASS, cwf::Direction::EAST, TREE);
	rules.addConnection(GRASS, cwf::Direction::SOUTH, TREE);
	rules.addConnection(GRASS, cwf::Direction::WEST, TREE);

	// WATER can only connect to WATER and SAND
	rules.addConnection(WATER, cwf::Direction::NORTH, WATER);
	rules.addConnection(WATER, cwf::Direction::EAST, WATER);
	rules.addConnection(WATER, cwf::Direction::SOUTH, WATER);
	rules.addConnection(WATER, cwf::Direction::WEST, WATER);

	rules.addConnection(WATER, cwf::Direction::NORTH, SAND);
	rules.addConnection(WATER, cwf::Direction::EAST, SAND);
	rules.addConnection(WATER, cwf::Direction::SOUTH, SAND);
	rules.addConnection(WATER, cwf::Direction::WEST, SAND);

	// SAND can connect to everything
	rules.addConnection(SAND, cwf::Direction::NORTH, SAND);
	rules.addConnection(SAND, cwf::Direction::EAST, SAND);
	rules.addConnection(SAND, cwf::Direction::SOUTH, SAND);
	rules.addConnection(SAND, cwf::Direction::WEST, SAND);

	rules.addConnection(SAND, cwf::Direction::NORTH, GRASS);
	rules.addConnection(SAND, cwf::Direction::EAST, GRASS);
	rules.addConnection(SAND, cwf::Direction::SOUTH, GRASS);
	rules.addConnection(SAND, cwf::Direction::WEST, GRASS);

	rules.addConnection(SAND, cwf::Direction::NORTH, WATER);
	rules.addConnection(SAND, cwf::Direction::EAST, WATER);
	rules.addConnection(SAND, cwf::Direction::SOUTH, WATER);
	rules.addConnection(SAND, cwf::Direction::WEST, WATER);
}

int main()
{
	// Clay setup
	setupClay();

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(1280, 800, "Wave Function Collapse Example");
	SearchAndSetResourceDir("resources");

	// Load pattern from file
	cwf::TileRules rules;
	cwf::Pattern pattern;
	try
	{
		pattern = cwf::PatternLoader::loadPatternFromFile("resources/pattern_example.txt");
		rules.addTileMapping(pattern.tileMapping);
		rules.learnPattern(pattern.charPattern);
	}
	catch (const std::exception &e)
	{
		// If pattern file doesn't exist, use default rules
		setupWFCRules(rules);
	}

	// Create WFC grid
	cwf::Grid grid(40, 30); // 20x20 grid

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
	float cellSize = 30.0f; // Size of each tile
	float offsetX = 100.0f; // Offset from left
	float offsetY = 100.0f; // Offset from top

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
			grid = cwf::Grid(40, 30);
			grid.initialize(possibleStates, rules);
			grid.setVisuals(visuals);
			isGenerating = false;
		}

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

		// Draw the grid
		grid.draw(cellSize, offsetX, offsetY);

		// Draw instructions
		DrawText("Space: Start/Pause Generation", 10, 10, 20, DARKGRAY);
		DrawText("R: Reset Grid", 10, 40, 20, DARKGRAY);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
