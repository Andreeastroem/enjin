#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay_renderer/renderer.h"

#include "raylib.h"
#include "StageManager/stageManager.h"

#include "StageManager/MenuStage/menuStage.h"
#include "StageManager/GameStage/gameStage.h"
#include "StageManager/GameOverStage/gameOverStage.h"

#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#include <memory>

struct windowSize
{
	static const u_int height = 1080;
	static const u_int width = 1920;
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

int main()
{
	// Clay setup
	Clay_Context *clayContext = setupClay();

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
	InitWindow(windowSize::width, windowSize::height, "Enjin");
	SetExitKey(KEY_NULL);
	SearchAndSetResourceDir("resources");

	Font fonts[1];
	// NOTE: After SearchAndSetResourceDir("resources") the working directory is the resources folder.
	// Load the font relative to the new CWD (no leading "resources/") so it actually loads.
	fonts[0] = LoadFontEx("AldotheApache.ttf", 48, 0, 400);

	Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);
	Clay_SetDebugModeEnabled(false);

	StageManager stageManager;
	stageManager.addStage(std::make_unique<MenuStage>(clayContext, fonts));
	stageManager.addStage(std::make_unique<GameStage>(clayContext, fonts));
	stageManager.addStage(std::make_unique<GameOverStage>(clayContext, fonts));
	stageManager.setCurrentStage("Menu");

	while (!WindowShouldClose())
	{
		Vector2 mp = GetMousePosition();
		Clay_SetPointerState(Clay_Vector2{mp.x, mp.y}, IsMouseButtonDown(MOUSE_LEFT_BUTTON));
		if (IsWindowResized())
		{
			Clay_SetLayoutDimensions(Clay_Dimensions{(float)GetScreenWidth(), (float)GetScreenHeight()});
		}

		stageManager.update(GetFrameTime());

		// Drawing
		BeginDrawing();
		ClearBackground(RAYWHITE);

		stageManager.render();

		EndDrawing();

		if (stageManager.exitRequested())
		{
			break;
		}
	}

	// Cleanup
	Clay_Raylib_Close();
	return 0;
}
