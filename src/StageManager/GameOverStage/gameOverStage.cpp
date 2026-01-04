#include "gameOverStage.h"

#include "clay_renderer/renderer.h"

#include <cstring>

namespace
{
    bool ClayButton(const char *label)
    {
        bool clicked = false;

        Clay_String dyn = {.isStaticallyAllocated = false, .length = (int)std::strlen(label), .chars = label};
        CLAY_AUTO_ID({.backgroundColor = {55, 55, 55, 255},
                      .cornerRadius = CLAY_CORNER_RADIUS(6),
                      .layout = {.padding = CLAY_PADDING_ALL(12),
                                 .sizing = {.width = CLAY_SIZING_FIXED(260), .height = CLAY_SIZING_FIT(0, 9999)},
                                 .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER}}})
        {
            CLAY_TEXT(dyn, CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 28, .textColor = {255, 255, 255, 255}}));
            if (Clay_Hovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                clicked = true;
            }
        }
        return clicked;
    }
}

GameOverStage::GameOverStage(Clay_Context *clayContext_, Font *fonts_)
    : clayContext(clayContext_),
      fonts(fonts_)
{
    name = "GameOver";
}

void GameOverStage::update(float deltaTime)
{
    (void)deltaTime;

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER))
    {
        requestStageChange("Menu");
    }
}

void GameOverStage::render()
{
    Clay_BeginLayout();

    CLAY(CLAY_ID("GameOverRoot"), {.layout = {.sizing = {.width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_GROW(1)}}})
    {
        CLAY(CLAY_ID("GameOverPanel"), {.backgroundColor = {35, 35, 35, 230},
                                        .cornerRadius = CLAY_CORNER_RADIUS(10),
                                        .floating = {.attachTo = CLAY_ATTACH_TO_ROOT,
                                                     .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
                                                     .zIndex = 100,
                                                     .attachPoints = {CLAY_ATTACH_POINT_CENTER_CENTER, CLAY_ATTACH_POINT_CENTER_CENTER},
                                                     .offset = {0, 0}},
                                        .layout = {.padding = CLAY_PADDING_ALL(20),
                                                   .sizing = {.width = CLAY_SIZING_FIXED(520), .height = CLAY_SIZING_FIT(0, 9999)},
                                                   .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                                   .childGap = 14,
                                                   .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_TOP}}})
        {
            CLAY_TEXT(CLAY_STRING("Game Over"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 44, .textColor = {255, 255, 255, 255}}));
            CLAY_TEXT(CLAY_STRING("Press Enter/Esc or click below"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 18, .textColor = {210, 210, 210, 255}}));

            if (ClayButton("Back to Menu"))
            {
                requestStageChange("Menu");
            }
        }
    }

    Clay_Raylib_Render(Clay_EndLayout(), fonts);
}
