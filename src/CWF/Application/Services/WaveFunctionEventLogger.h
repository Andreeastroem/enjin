#pragma once

#include <iostream>
#include "../../Domain/Events/EventDispatcher.h"
#include "../../Domain/Events/WaveFunctionEvents.h"

namespace cwf::application
{

    class WaveFunctionEventLogger
    {
    public:
        WaveFunctionEventLogger();

    private:
        void registerEventHandlers();
    };

} // namespace cwf::application