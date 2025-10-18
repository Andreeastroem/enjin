#pragma once

#include <string>

#include "clay.h"
#include "raylib.h"

#include "CWF/grid.h"

namespace ui
{
    struct UiState
    {
        bool showSavePrompt = false;
        std::string saveFilename = "output.pattern";
        bool saveAttempted = false;
        bool saveSuccess = false;
    };

    inline void Init(UiState &state)
    {
        state.showSavePrompt = false;
        state.saveFilename = "output.pattern";
        state.saveAttempted = false;
        state.saveSuccess = false;
    }

    inline void RenderText(Clay_String text)
    {
        CLAY_AUTO_ID({.layout = {.padding = CLAY_PADDING_ALL(16)}})
        {
            CLAY_TEXT(text, CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 16, .textColor = {255, 255, 255, 255}}));
        }
    }

    inline void SaveModal(cwf::Grid &grid, UiState &state)
    {
        if (!state.showSavePrompt)
            return;

        CLAY(CLAY_ID("SaveOverlay"), {.backgroundColor = {0, 0, 0, 128},
                                      .floating = {.attachTo = CLAY_ATTACH_TO_ROOT,
                                                   .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
                                                   .zIndex = 1000,
                                                   .attachPoints = {CLAY_ATTACH_POINT_CENTER_CENTER, CLAY_ATTACH_POINT_CENTER_CENTER},
                                                   .offset = {0, 0}},
                                      .layout = {.sizing = {.width = CLAY_SIZING_PERCENT(1.0f), .height = CLAY_SIZING_PERCENT(1.0f)}}})
        {
            CLAY(CLAY_ID("SaveDialogPanel"), {.backgroundColor = {35, 35, 35, 255},
                                              .cornerRadius = CLAY_CORNER_RADIUS(8),
                                              .floating = {.attachTo = CLAY_ATTACH_TO_ROOT,
                                                           .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
                                                           .zIndex = 1001,
                                                           .attachPoints = {CLAY_ATTACH_POINT_CENTER_CENTER, CLAY_ATTACH_POINT_CENTER_CENTER},
                                                           .offset = {0, 0}},
                                              .layout = {.padding = CLAY_PADDING_ALL(16),
                                                         .sizing = {.width = CLAY_SIZING_FIXED(540), .height = CLAY_SIZING_FIXED(220)},
                                                         .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                                         .childGap = 12,
                                                         .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_TOP}}})
            {
                RenderText(CLAY_STRING("Save As"));

                CLAY_AUTO_ID({.layout = {.sizing = {.width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_FIT(0, 9999)}}})
                {
                    CLAY_TEXT(CLAY_STRING("Filename"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 18, .textColor = {230, 230, 230, 255}}));
                }

                CLAY(CLAY_ID("FilenameBox"), {.backgroundColor = {55, 55, 55, 255},
                                              .border = {.color = {200, 200, 200, 255}, .width = CLAY_BORDER_ALL(1)},
                                              .cornerRadius = CLAY_CORNER_RADIUS(4),
                                              .layout = {.padding = CLAY_PADDING_ALL(10),
                                                         .sizing = {.width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_FIXED(44)},
                                                         .childAlignment = {CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER}}})
                {
                    Clay_String dyn = {.isStaticallyAllocated = false, .length = (int)state.saveFilename.size(), .chars = state.saveFilename.c_str()};
                    CLAY_TEXT(dyn, CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 20, .textColor = {255, 255, 255, 255}}));
                }

                CLAY(CLAY_ID("ButtonsRow"), {.layout = {.sizing = {.width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_FIT(0, 9999)},
                                                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                                        .childAlignment = {CLAY_ALIGN_X_RIGHT, CLAY_ALIGN_Y_TOP},
                                                        .childGap = 12}})
                {
                    CLAY(CLAY_ID("CancelButton"), {.backgroundColor = {90, 90, 90, 255},
                                                   .cornerRadius = CLAY_CORNER_RADIUS(4),
                                                   .layout = {.padding = CLAY_PADDING_ALL(10), .sizing = {.width = CLAY_SIZING_FIT(0, 9999), .height = CLAY_SIZING_FIT(0, 9999)}}})
                    {
                        CLAY_TEXT(CLAY_STRING("Cancel"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 18, .textColor = {255, 255, 255, 255}}));
                        if (Clay_Hovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        {
                            state.showSavePrompt = false;
                            state.saveAttempted = false;
                        }
                    }

                    CLAY(CLAY_ID("SaveButton"), {.backgroundColor = {0, 120, 215, 255},
                                                 .cornerRadius = CLAY_CORNER_RADIUS(4),
                                                 .layout = {.padding = CLAY_PADDING_ALL(10), .sizing = {.width = CLAY_SIZING_FIT(0, 9999), .height = CLAY_SIZING_FIT(0, 9999)}}})
                    {
                        CLAY_TEXT(CLAY_STRING("Save"), CLAY_TEXT_CONFIG({.fontId = 0, .fontSize = 18, .textColor = {255, 255, 255, 255}}));
                        if (Clay_Hovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        {
                            state.saveSuccess = grid.saveToFile(state.saveFilename);
                            state.saveAttempted = true;
                            if (state.saveSuccess)
                            {
                                state.showSavePrompt = false;
                            }
                        }
                    }
                }

                if (state.saveAttempted)
                {
                    CLAY_AUTO_ID({.layout = {.sizing = {.width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_FIT(0, 9999)}}})
                    {
                        if (state.saveSuccess)
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

    inline Clay_RenderCommandArray SideBar(Clay_Context *context, cwf::Grid &grid, UiState &state)
    {
        (void)context;
        Clay_BeginLayout();

        Clay_Sizing layoutExpand = {.width = CLAY_SIZING_PERCENT(0.20f), .height = CLAY_SIZING_GROW(1)};
        Clay_Color backgroundColor = {90, 90, 90, 200};

        CLAY(CLAY_ID("root"), {.layout = {.sizing = {.width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_GROW(1)}}})
        {
            CLAY(CLAY_ID("Container"), {.backgroundColor = backgroundColor,
                                        .layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = layoutExpand, .childGap = 16}})
            {
                RenderText(CLAY_STRING("Space: Start/Pause Generation"));
                RenderText(CLAY_STRING("R: Reset Grid"));
                RenderText(CLAY_STRING("Arrows/WASD or RMB drag: Pan"));
                RenderText(CLAY_STRING("Mouse wheel: Zoom"));
                RenderText(CLAY_STRING("M: Toggle minimap visibility"));
                RenderText(CLAY_STRING("N: Toggle minimap size"));
                RenderText(CLAY_STRING("Ctrl+S: Save As"));
            }
        }

        // Render save modal on top
        SaveModal(grid, state);
        Clay_RenderCommandArray renderCommands = Clay_EndLayout();
        return renderCommands;
    }

    inline void HandleShortcuts(UiState &state)
    {
        bool ctrlDown = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
        if (!state.showSavePrompt && ctrlDown && IsKeyPressed(KEY_S))
        {
            state.showSavePrompt = true;
            state.saveAttempted = false;
        }
    }

    inline void HandleSaveDialogInput(UiState &state, cwf::Grid &grid)
    {
        if (!state.showSavePrompt)
            return;

        if (IsKeyPressed(KEY_ENTER))
        {
            state.saveSuccess = grid.saveToFile(state.saveFilename);
            state.saveAttempted = true;
            if (state.saveSuccess)
            {
                state.showSavePrompt = false;
            }
        }
        if (IsKeyPressed(KEY_ESCAPE))
        {
            state.showSavePrompt = false;
            state.saveAttempted = false;
        }
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            if (!state.saveFilename.empty())
            {
                state.saveFilename.pop_back();
            }
        }
        int key = 0;
        while ((key = GetCharPressed()) != 0)
        {
            if (key >= 32 && key < 127)
            {
                char c = (char)key;
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.')
                {
                    state.saveFilename.push_back(c);
                }
            }
        }
    }
}
