#include "EventDispatcher.h"

namespace cwf::domain
{
    EventDispatcher::~EventDispatcher()
    {
    }
    void EventDispatcher::dispatch(const DomainEvent &event)
    {
        auto typeIndex = std::type_index(typeid(event));
        auto it = handlers_.find(typeIndex);
        if (it != handlers_.end())
        {
            for (const auto &handler : it->second)
            {
                handler(event);
            }
        }
    }

    EventDispatcher &EventBus::getInstance()
    {
        static EventDispatcher instance;
        return instance;
    }

} // namespace cwf::domain