#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay_renderer/renderer.h"

#include "CWF/Application/Services/TileVisualService.h"
#include "CWF/Application/Services/WaveFunctionCollapseService.h"
#include "CWF/Application/Services/PatternManagementService.h"
#include "CWF/Infrastructure/Persistence/File/FilePatternRepository.h"
#include "CWF/Infrastructure/Persistence/File/FileGridRepository.h"

#include "raylib.h"

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

int main()
{
	// Clay setup
	setupClay();

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(1280, 800, "Wave Function Collapse Example");
	SearchAndSetResourceDir("resources");

	// Create repositories and services
	auto gridRepo = std::make_shared<cwf::infrastructure::FileGridRepository>("resources/grids/");
	auto patternRepo = std::make_shared<cwf::infrastructure::FilePatternRepository>("patterns/");
	cwf::application::WaveFunctionCollapseService collapseService(gridRepo, patternRepo);
	cwf::application::PatternManagementService patternService(patternRepo);
	auto &visualService = cwf::application::TileVisualService::getInstance();

	// Load pattern
	auto pattern = patternService.loadPattern("pattern_example");

	// Set up visuals
	visualService.setVisual(cwf::domain::TileId(0), cwf::domain::TileVisual(GREEN));
	visualService.setVisual(cwf::domain::TileId(1), cwf::domain::TileVisual(BLUE));
	visualService.setVisual(cwf::domain::TileId(2), cwf::domain::TileVisual(BEIGE));
	visualService.setVisual(cwf::domain::TileId(3), cwf::domain::TileVisual(DARKGREEN));

	// Create grid and initialize it
	auto grid = std::make_unique<cwf::domain::Grid>(30, 20);
	// Convert pattern to domain pattern
	collapseService.initializeGrid(*grid, *pattern);

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
			grid = std::make_unique<cwf::domain::Grid>(30, 20);
			collapseService.initializeGrid(*grid, *pattern);
			isGenerating = false;
		}

		if (isGenerating)
		{
			// Perform one step of the wave function collapse
			if (!collapseService.performCollapseStep(*grid, *pattern))
			{
				isGenerating = false; // Stop when complete
			}
		}

		// Drawing
		BeginDrawing();
		ClearBackground(RAYWHITE);

		// Draw the grid
		for (int y = 0; y < grid->height(); ++y)
		{
			for (int x = 0; x < grid->width(); ++x)
			{
				const auto &tile = grid->getTile(cwf::domain::Position(x, y));
				if (tile.isCollapsed())
				{
					float drawX = offsetX + x * cellSize;
					float drawY = offsetY + y * cellSize;
					if (auto visual = visualService.getVisual(tile.currentState()))
					{
						DrawRectangle(drawX, drawY, cellSize, cellSize, visual->color);
						if (visual->texture)
						{
							DrawTextureRec(*visual->texture, visual->sourceRect,
										   {drawX, drawY}, WHITE);
						}
					}
				}
			}
		}

		// Draw instructions
		DrawText("Space: Start/Pause Generation", 10, 10, 20, DARKGRAY);
		DrawText("R: Reset Grid", 10, 40, 20, DARKGRAY);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
