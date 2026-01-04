#pragma once

#include "../stage.h"

#include "clay.h"

class MenuStage : public Stage
{
public:
    MenuStage();

    void update(float deltatime);
    void render();

private:
};