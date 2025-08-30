#pragma once

#include "stage.h"

#include <map>
#include <string>

class StageManager
{
public:
    StageManager();
    ~StageManager();

    void update(float deltaTime);
    void render();

    void addStage(Stage *stage);
    void removeStage(std::string name);

    Stage *getCurrentStage();
    Stage *getStageByName(const std::string &name);

private:
    Stage *currentStage;
    std::map<std::string, Stage *> possibleStages;
};