#pragma once

#include "../stage.h"

class MainStage : public Stage
{
public:
    MainStage();
    ~MainStage();

    void update(float deltaTime);
    void render();
};
