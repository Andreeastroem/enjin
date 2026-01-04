#pragma once

#include "../stage.h"

#include "clay.h"
#include "raylib.h"

// GameStage is the in-game ARPG stage.
// It renders a simple HUD and a pause overlay (Esc).

class GameStage : public Stage
{
public:
    GameStage(Clay_Context *clayContext, Font *fonts);

    void update(float deltaTime) override;
    void render() override;

protected:
    void onEnter() override;

private:
private:
    Clay_Context *clayContext;
    Font *fonts;

    bool paused = false;
    float health01 = 1.0f;
    float mana01 = 1.0f;
};
