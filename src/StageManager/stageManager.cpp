#include "stageManager.h"

StageManager::StageManager()
{
    currentStage = nullptr;
}

StageManager::~StageManager()
{
    // unique_ptr map cleans up automatically
}

void StageManager::update(float deltaTime)
{
    if (queuedStage.has_value())
    {
        setCurrentStage(*queuedStage);
        queuedStage.reset();
    }
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

void StageManager::addStage(std::unique_ptr<Stage> newStage)
{
    if (newStage)
    {
        newStage->stageManager = this;

        std::string stageName = newStage->name;
        if (stageName.empty())
        {
            stageName = "(unnamed)";
            newStage->name = stageName;
        }

        // Replace existing stage if it exists.
        auto it = possibleStages.find(stageName);
        if (it != possibleStages.end() && currentStage == it->second.get())
        {
            currentStage->onExit();
            currentStage = nullptr;
        }

        possibleStages[stageName] = std::move(newStage);

        // If this is the first stage ever added, make it current.
        if (!currentStage)
        {
            setCurrentStage(stageName);
        }
    }
}

void StageManager::removeStage(std::string name)
{
    Stage *stage = getStageByName(name);
    if (stage)
    {
        if (currentStage == stage)
        {
            currentStage->onExit();
            currentStage = nullptr;
        }
        possibleStages.erase(stage->name);
    }
}

bool StageManager::setCurrentStage(const std::string &name)
{
    Stage *next = getStageByName(name);
    if (!next)
    {
        return false;
    }

    if (currentStage == next)
    {
        return true;
    }

    if (currentStage)
    {
        currentStage->onExit();
    }

    currentStage = next;
    currentStage->onEnter();
    return true;
}

void StageManager::queueStageChange(const std::string &name)
{
    queuedStage = name;
}

void StageManager::requestExit()
{
    shouldExit = true;
}

bool StageManager::exitRequested() const
{
    return shouldExit;
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
        return it->second.get();
    }
    return nullptr;
}