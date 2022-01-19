#pragma once
#include <memory>

namespace test {

    class Event;

    class EventHandler {
    public:
        virtual ~EventHandler() = default;
        virtual void HandleEvent(const std::shared_ptr<Event> &event) = 0;
    };
}