#include "stage.h"

#include "stageManager.h"

Stage::~Stage() = default;

void Stage::requestStageChange(const std::string &stageName)
{
    if (!stageManager)
    {
        return;
    }
    stageManager->queueStageChange(stageName);
}

void Stage::requestExit()
{
    if (!stageManager)
    {
        return;
    }
    stageManager->requestExit();
}
