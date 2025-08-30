#include "stageManager.h"

StageManager::StageManager()
{
    currentStage = nullptr;
}

StageManager::~StageManager()
{
    for (auto &pair : possibleStages)
    {
        delete pair.second;
    }
}

void StageManager::update(float deltaTime)
{
    if (currentStage)
    {
        currentStage->update(deltaTime);
    }
}

void StageManager::render()
{
    if (currentStage)
    {
        currentStage->render();
    }
}

void StageManager::addStage(Stage *newStage)
{
    if (newStage)
    {
        Stage *stage = getStageByName(newStage->name);
        if (stage)
        {
        }
        possibleStages[newStage->name] = newStage;
    }
}

void StageManager::removeStage(std::string name)
{
    Stage *stage = getStageByName(name);
    if (stage)
    {
        possibleStages.erase(stage->name);
        if (currentStage == stage)
        {
            currentStage = nullptr;
        }
        delete stage;
    }
}

Stage *StageManager::getCurrentStage()
{
    return currentStage;
}

Stage *StageManager::getStageByName(const std::string &name)
{
    auto it = possibleStages.find(name);
    if (it != possibleStages.end())
    {
        return it->second;
    }
    return nullptr;
}