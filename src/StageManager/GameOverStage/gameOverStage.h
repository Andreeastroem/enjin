#pragma once

#include "../stage.h"

#include "clay.h"
#include "raylib.h"

class GameOverStage : public Stage
{
public:
    GameOverStage(Clay_Context *clayContext, Font *fonts);

    void update(float deltaTime) override;
    void render() override;

private:
    Clay_Context *clayContext;
    Font *fonts;
};
