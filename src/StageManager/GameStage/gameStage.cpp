#include "gameStage.h"

#include "clay_renderer/renderer.h"

namespace
{
    void HudBar(float health01, float mana01)
    {
        // Bottom-centered HUD with left HP, center hotbar, right Mana.
        CLAY(CLAY_ID("HudRoot"), {.floating = {.attachTo = CLAY_ATTACH_TO_ROOT,
                                               .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_PASSTHROUGH,
                                               .zIndex = 10,
                                               .attachPoints = {CLAY_ATTACH_POINT_CENTER_BOTTOM, CLAY_ATTACH_POINT_CENTER_BOTTOM},
                                               .offset = {0, -16}},
                                  .layout = {.sizing = {.width = CLAY_SIZING_FIXED(820), .height = CLAY_SIZING_FIXED(96)},
                                             .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                             .childGap = 12,
                                             .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER}}})
        {
            // HP
            CLAY(CLAY_ID("HudHp"), {.backgroundColor = {35, 35, 35, 230},
                                    .cornerRadius = CLAY_CORNER_RADIUS(8),
                                    .layout = {.padding = CLAY_PADDING_ALL(10),
                                               .sizing = {.width = CLAY_SIZING_FIXED(180), .height = CLAY_SIZING_GROW(1)},
                                               .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                               .childGap = 6,
                                               .childAlignment = {CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER}}})
            {
                CLAY_TEXT(CLAY_STRING("HP"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 16, .textColor = {230, 230, 230, 255}}));
                CLAY(CLAY_ID("HudHpBarBg"), {.backgroundColor = {55, 55, 55, 255},
                                             .cornerRadius = CLAY_CORNER_RADIUS(6),
                                             .layout = {.sizing = {.width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_FIXED(18)}}})
                {
                    CLAY(CLAY_ID("HudHpBarFg"), {.backgroundColor = {200, 60, 60, 255},
                                                 .cornerRadius = CLAY_CORNER_RADIUS(6),
                                                 .layout = {.sizing = {.width = CLAY_SIZING_PERCENT(health01), .height = CLAY_SIZING_GROW(1)}}})
                    {
                    }
                }
            }

            // Hotbar
            CLAY(CLAY_ID("HudHotbar"), {.backgroundColor = {35, 35, 35, 230},
                                        .cornerRadius = CLAY_CORNER_RADIUS(8),
                                        .layout = {.padding = CLAY_PADDING_ALL(10),
                                                   .sizing = {.width = CLAY_SIZING_FIXED(420), .height = CLAY_SIZING_GROW(1)},
                                                   .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                                   .childGap = 8,
                                                   .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER}}})
            {
                for (int i = 0; i < 5; i++)
                {
                    CLAY_AUTO_ID({.backgroundColor = {55, 55, 55, 255},
                                  .cornerRadius = CLAY_CORNER_RADIUS(6),
                                  .layout = {.sizing = {.width = CLAY_SIZING_FIXED(64), .height = CLAY_SIZING_FIXED(64)},
                                             .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER}}})
                    {
                        (void)i;
                    }
                }
            }

            // Mana
            CLAY(CLAY_ID("HudMana"), {.backgroundColor = {35, 35, 35, 230},
                                      .cornerRadius = CLAY_CORNER_RADIUS(8),
                                      .layout = {.padding = CLAY_PADDING_ALL(10),
                                                 .sizing = {.width = CLAY_SIZING_FIXED(180), .height = CLAY_SIZING_GROW(1)},
                                                 .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                                 .childGap = 6,
                                                 .childAlignment = {CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER}}})
            {
                CLAY_TEXT(CLAY_STRING("Mana"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 16, .textColor = {230, 230, 230, 255}}));
                CLAY(CLAY_ID("HudManaBarBg"), {.backgroundColor = {55, 55, 55, 255},
                                               .cornerRadius = CLAY_CORNER_RADIUS(6),
                                               .layout = {.sizing = {.width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_FIXED(18)}}})
                {
                    CLAY(CLAY_ID("HudManaBarFg"), {.backgroundColor = {0, 120, 215, 255},
                                                   .cornerRadius = CLAY_CORNER_RADIUS(6),
                                                   .layout = {.sizing = {.width = CLAY_SIZING_PERCENT(mana01), .height = CLAY_SIZING_GROW(1)}}})
                    {
                    }
                }
            }
        }
    }

    bool PauseOverlay()
    {
        bool resumeClicked = false;
        bool exitClicked = false;

        CLAY(CLAY_ID("PauseOverlay"), {.backgroundColor = {0, 0, 0, 140},
                                       .floating = {.attachTo = CLAY_ATTACH_TO_ROOT,
                                                    .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
                                                    .zIndex = 1000,
                                                    .attachPoints = {CLAY_ATTACH_POINT_CENTER_CENTER, CLAY_ATTACH_POINT_CENTER_CENTER},
                                                    .offset = {0, 0}},
                                       .layout = {.sizing = {.width = CLAY_SIZING_PERCENT(1.0f), .height = CLAY_SIZING_PERCENT(1.0f)}}})
        {
            CLAY(CLAY_ID("PausePanel"), {.backgroundColor = {35, 35, 35, 245},
                                         .cornerRadius = CLAY_CORNER_RADIUS(10),
                                         .floating = {.attachTo = CLAY_ATTACH_TO_ROOT,
                                                      .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
                                                      .zIndex = 1001,
                                                      .attachPoints = {CLAY_ATTACH_POINT_CENTER_CENTER, CLAY_ATTACH_POINT_CENTER_CENTER},
                                                      .offset = {0, 0}},
                                         .layout = {.padding = CLAY_PADDING_ALL(20),
                                                    .sizing = {.width = CLAY_SIZING_FIXED(460), .height = CLAY_SIZING_FIT(0, 9999)},
                                                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                                    .childGap = 14,
                                                    .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_TOP}}})
            {
                CLAY_TEXT(CLAY_STRING("Paused"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 44, .textColor = {255, 255, 255, 255}}));

                CLAY(CLAY_ID("PauseResume"), {.backgroundColor = {55, 55, 55, 255},
                                              .cornerRadius = CLAY_CORNER_RADIUS(6),
                                              .layout = {.padding = CLAY_PADDING_ALL(12),
                                                         .sizing = {.width = CLAY_SIZING_FIXED(260), .height = CLAY_SIZING_FIT(0, 9999)},
                                                         .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER}}})
                {
                    CLAY_TEXT(CLAY_STRING("Resume"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 28, .textColor = {255, 255, 255, 255}}));
                    if (Clay_Hovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        resumeClicked = true;
                    }
                }

                CLAY(CLAY_ID("PauseExit"), {.backgroundColor = {55, 55, 55, 255},
                                            .cornerRadius = CLAY_CORNER_RADIUS(6),
                                            .layout = {.padding = CLAY_PADDING_ALL(12),
                                                       .sizing = {.width = CLAY_SIZING_FIXED(260), .height = CLAY_SIZING_FIT(0, 9999)},
                                                       .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER}}})
                {
                    CLAY_TEXT(CLAY_STRING("Exit"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 28, .textColor = {255, 255, 255, 255}}));
                    if (Clay_Hovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        exitClicked = true;
                    }
                }
            }
        }

        if (resumeClicked)
            return false;
        if (exitClicked)
            return true;
        return false;
    }
}

GameStage::GameStage(Clay_Context *clayContext_, Font *fonts_)
    : clayContext(clayContext_),
      fonts(fonts_)
{
    name = "Game";
}

void GameStage::onEnter()
{
    paused = false;
}

void GameStage::update(float deltaTime)
{
    (void)deltaTime;

    if (IsKeyPressed(KEY_ESCAPE))
    {
        paused = !paused;
    }

    if (paused)
    {
        // Keep it minimal: no gameplay updates while paused.
        return;
    }

    // Placeholder: simple values to show the HUD is live.
    if (IsKeyDown(KEY_ONE))
        health01 = 0.25f;
    if (IsKeyDown(KEY_TWO))
        health01 = 0.60f;
    if (IsKeyDown(KEY_THREE))
        health01 = 1.0f;
}

void GameStage::render()
{
    // Placeholder world rendering.
    DrawText("Game Stage", 30, 30, 24, DARKGRAY);
    DrawText("ESC: Pause", 30, 60, 18, DARKGRAY);

    Clay_BeginLayout();

    CLAY(CLAY_ID("GameRoot"), {.layout = {.sizing = {.width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_GROW(1)}}})
    {
        HudBar(health01, mana01);

        if (paused)
        {
            // PauseOverlay returns true if Exit clicked.
            bool exitClicked = PauseOverlay();
            if (exitClicked)
            {
                paused = false;
                requestStageChange("Menu");
            }

            // Also allow resume via clicking (handled above) or pressing ESC.
            // ESC toggling is handled in update().
        }
    }

    Clay_Raylib_Render(Clay_EndLayout(), fonts);
}
