#pragma once

#include <string>
class Stage
{
    friend class StageManager;

public:
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

protected:
    std::string name;
};