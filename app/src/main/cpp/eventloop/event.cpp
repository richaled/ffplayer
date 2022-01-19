#include "event.h"
#include "event_dispatcher.h"

namespace test{

    std::shared_ptr<Event> NewEvent(uint32_t what, const std::shared_ptr<EventHandler> &handler, const std::shared_ptr<EventDispatcher> &dispatcher) {
        auto event = std::make_shared<Event>(what);
        event->SetHandler(handler);
        event->SetDispatcher(dispatcher);
        return event;
    }

    void Event::Post() {
        std::shared_ptr<EventDispatcher> dispatcher = eventDispatcher_.lock();
        if(dispatcher){
            dispatcher->Register(shared_from_this());
        }
    }

    void Event::Cancel() {
        auto dispatcher = eventDispatcher_.lock();
        if(dispatcher){
            dispatcher->UnRegister(shared_from_this());
        }
    }
}