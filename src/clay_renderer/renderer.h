// renderer.h
#pragma once
#include "raylib.h"
#include "../clay.h"

// Function declarations
void Clay_Raylib_Initialize(int width, int height, const char *title, unsigned int flags);
void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands, Font *fonts);
void Clay_Raylib_Close();
Ray GetScreenToWorldPointWithZDistance(Vector2 position, Camera camera, int screenWidth, int screenHeight, float zDistance);

// Declare external variables
extern Camera Raylib_camera;