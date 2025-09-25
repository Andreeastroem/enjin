#include "WaveFunctionEventLogger.h"

namespace cwf::application
{

    WaveFunctionEventLogger::WaveFunctionEventLogger()
    {
        registerEventHandlers();
    }

    void WaveFunctionEventLogger::registerEventHandlers()
    {
        domain::EventBus::getInstance().subscribe<domain::TileCollapseEvent>(
            [](const domain::TileCollapseEvent &event)
            {
                std::cout << "Tile at position (" << event.position().x() << ", "
                          << event.position().y() << ") collapsed to state "
                          << event.state().value() << std::endl;
            });

        domain::EventBus::getInstance().subscribe<domain::EntropyChangeEvent>(
            [](const domain::EntropyChangeEvent &event)
            {
                std::cout << "Tile at position (" << event.position().x() << ", "
                          << event.position().y() << ") entropy changed from "
                          << event.oldEntropy() << " to " << event.newEntropy() << std::endl;
            });

        domain::EventBus::getInstance().subscribe<domain::GridGenerationStartEvent>(
            [](const domain::GridGenerationStartEvent &event)
            {
                std::cout << "Starting grid generation: " << event.width() << "x"
                          << event.height() << " using pattern " << event.patternId() << std::endl;
            });

        domain::EventBus::getInstance().subscribe<domain::GridGenerationCompleteEvent>(
            [](const domain::GridGenerationCompleteEvent &event)
            {
                std::cout << "Grid generation "
                          << (event.success() ? "completed successfully" : "failed")
                          << (event.message().empty() ? "" : ": " + event.message())
                          << std::endl;
            });
    }

} // namespace cwf::application