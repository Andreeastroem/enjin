#pragma once

#include "stage.h"

#include <map>
#include <memory>
#include <optional>
#include <string>

class StageManager
{
public:
    StageManager();
    ~StageManager();

    void update(float deltaTime);
    void render();

    void addStage(std::unique_ptr<Stage> stage);
    void removeStage(std::string name);

    bool setCurrentStage(const std::string &name);
    void queueStageChange(const std::string &name);

    void requestExit();
    bool exitRequested() const;

    Stage *getCurrentStage();
    Stage *getStageByName(const std::string &name);

private:
    Stage *currentStage;
    std::optional<std::string> queuedStage;
    bool shouldExit = false;
    std::map<std::string, std::unique_ptr<Stage>> possibleStages;
};