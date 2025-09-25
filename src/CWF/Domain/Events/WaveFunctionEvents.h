#pragma once

#include <string>
#include "../ValueObjects/Position.h"
#include "../ValueObjects/TileId.h"

namespace cwf::domain
{

    class DomainEvent
    {
    public:
        virtual ~DomainEvent() = default;
    };

    class TileCollapseEvent : public DomainEvent
    {
    public:
        TileCollapseEvent(Position pos, TileId state);

        Position position() const;
        TileId state() const;

    private:
        Position position_;
        TileId state_;
    };

    class EntropyChangeEvent : public DomainEvent
    {
    public:
        EntropyChangeEvent(Position pos, size_t oldEntropy, size_t newEntropy);

        Position position() const;
        size_t oldEntropy() const;
        size_t newEntropy() const;

    private:
        Position position_;
        size_t old_entropy_;
        size_t new_entropy_;
    };

    class GridGenerationStartEvent : public DomainEvent
    {
    public:
        GridGenerationStartEvent(int width, int height, std::string patternId);

        int width() const;
        int height() const;
        const std::string &patternId() const;

    private:
        int width_;
        int height_;
        std::string pattern_id_;
    };

    class GridGenerationCompleteEvent : public DomainEvent
    {
    public:
        GridGenerationCompleteEvent(bool success, std::string message = "");

        bool success() const;
        const std::string &message() const;

    private:
        bool success_;
        std::string message_;
    };

} // namespace cwf::domain