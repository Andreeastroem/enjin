/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay_renderer/renderer.h"

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

Clay_LayoutConfig layoutElement = Clay_LayoutConfig{.padding = {5}, .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}};

Clay_LayoutConfig boxElement = Clay_LayoutConfig{.padding = {5}, .sizing = {.width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(100)}};

Clay_RenderCommandArray renderClay()
{
	Clay_BeginLayout();
	CLAY({.layout = layoutElement, .backgroundColor = {255, 255, 255, 0}})
	{
		CLAY({.layout = boxElement, .backgroundColor = {255, 0, 0, 255}})
		{
			CLAY_TEXT(CLAY_STRING("hello"), CLAY_TEXT_CONFIG({.fontId = 0}));
		}
		CLAY_TEXT(CLAY_STRING("hello"), CLAY_TEXT_CONFIG({.fontId = 0}));
	}
	return Clay_EndLayout();
}

int main()
{
	// Clay setup
	setupClay();

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1280, 800, "Hello Raylib");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("wabbit_alpha.png");

	StageManager stageManager;

	Font fonts[1];
	fonts[0] = LoadFontEx("resources/AldotheApache.ttf", 32, 0, 400);
	SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);

	// game loop
	while (!WindowShouldClose()) // run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		Clay_RenderCommandArray renderCommands = renderClay();

		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(WHITE);

		// draw some text using the default font
		DrawText("Hello Raylib", 200, 200, 20, WHITE);

		// draw our texture to the screen
		DrawTexture(wabbit, 400, 200, WHITE);

		Clay_Raylib_Render(renderCommands, fonts);

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadTexture(wabbit);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
