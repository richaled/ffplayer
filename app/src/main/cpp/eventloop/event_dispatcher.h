#pragma once

#include <atomic>
#include <thread>
#include "list"

extern "C" {
    #include <event2/event.h>
}

namespace test {
    class Event;
    class EventDispatcher {
    public:
        EventDispatcher();

        virtual ~EventDispatcher();

        void Run();

        void Stop();

        void Release();

        bool IsRunning() const {
            return running_;
        }

        void Register(const std::shared_ptr<Event> &event);
        void UnRegister(const std::shared_ptr<Event> &event);

    private:
        using BreakEventKey = std::pair<EventDispatcher *, struct event *>;
        static void OnLoopEvent(evutil_socket_t sock, short what, void *arg);
        static void OnQuitEvent(evutil_socket_t sock, short what, void *arg);

        void DispatchEvent(const std::shared_ptr<Event> &event);

    private:
        std::atomic_bool running_ = {false};
        event_base *event_base_ = nullptr;

        std::thread event_thread_;
        std::mutex mutex_;

        //存放event的集合
        std::list<std::shared_ptr<Event>> events_;
        struct event *notify_event_ = nullptr;

    };

}

