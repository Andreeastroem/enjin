#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include "WaveFunctionEvents.h"

namespace cwf::domain
{

    class EventDispatcher
    {
    public:
        ~EventDispatcher();

        template <typename TEvent>
        void subscribe(std::function<void(const TEvent &)> handler)
        {
            auto typeIndex = std::type_index(typeid(TEvent));
            handlers_[typeIndex].push_back(
                [handler](const DomainEvent &event)
                {
                    handler(static_cast<const TEvent &>(event));
                });
        }

        void dispatch(const DomainEvent &event);

    private:
        std::unordered_map<
            std::type_index,
            std::vector<std::function<void(const DomainEvent &)>>>
            handlers_;
    };

    // Singleton event dispatcher
    class EventBus
    {
    public:
        static EventDispatcher &getInstance();
    };

} // namespace cwf::domain