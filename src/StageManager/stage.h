#pragma once

#include <string>

class StageManager;
class Stage
{
    friend class StageManager;

public:
    virtual ~Stage() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    const std::string &getName() const { return name; }

protected:
    virtual void onEnter() {}
    virtual void onExit() {}

    void requestStageChange(const std::string &stageName);
    void requestExit();

protected:
    std::string name;

private:
    StageManager *stageManager = nullptr;
};