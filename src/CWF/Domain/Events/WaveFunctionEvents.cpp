#include "WaveFunctionEvents.h"

namespace cwf::domain
{
    // TileCollapseEvent
    TileCollapseEvent::TileCollapseEvent(Position pos, TileId state)
        : position_(pos), state_(state) {}

    Position TileCollapseEvent::position() const { return position_; }

    TileId TileCollapseEvent::state() const { return state_; }

    // EntropyChangeEvent
    EntropyChangeEvent::EntropyChangeEvent(Position pos, size_t oldEntropy, size_t newEntropy)
        : position_(pos), old_entropy_(oldEntropy), new_entropy_(newEntropy) {}

    Position EntropyChangeEvent::position() const { return position_; }

    size_t EntropyChangeEvent::oldEntropy() const { return old_entropy_; }

    size_t EntropyChangeEvent::newEntropy() const { return new_entropy_; }

    // GridGenerationStartEvent
    GridGenerationStartEvent::GridGenerationStartEvent(int width, int height, std::string patternId)
        : width_(width), height_(height), pattern_id_(std::move(patternId)) {}

    int GridGenerationStartEvent::width() const { return width_; }

    int GridGenerationStartEvent::height() const { return height_; }

    const std::string &GridGenerationStartEvent::patternId() const { return pattern_id_; }

    // GridGenerationCompleteEvent
    GridGenerationCompleteEvent::GridGenerationCompleteEvent(bool success, std::string message)
        : success_(success), message_(std::move(message)) {}

    bool GridGenerationCompleteEvent::success() const { return success_; }

    const std::string &GridGenerationCompleteEvent::message() const { return message_; }

} // namespace cwf::domain