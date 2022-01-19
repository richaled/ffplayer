#include <mutex>
#include "event_dispatcher.h"

extern "C" {
    #include <event2/thread.h>
}
#include "event.h"
#include "event_handler.h"

namespace test {
    EventDispatcher::EventDispatcher() {
        std::once_flag flag;
        std::call_once(flag, [] {
            evthread_use_pthreads();
        });
    }

    EventDispatcher::~EventDispatcher() {
        if (event_thread_.joinable()){
            event_thread_.join();
        }
    }

    void EventDispatcher::Run() {
        auto config = event_config_new();
        event_config_set_flag(config,EVENT_BASE_FLAG_PRECISE_TIMER);
        event_base_ = event_base_new_with_config(config);
        event_config_free(config);
        notify_event_ = event_new(event_base_, -1, EV_PERSIST | EV_READ, OnLoopEvent, this);
        event_add(notify_event_, nullptr);
        //处理event事件
        auto func = [&](){
            event_base_dispatch(event_base_);
        };
        running_ = true;
        event_thread_ = std::thread(func);
    }

    void EventDispatcher::Stop() {
        if(!running_){
            return;
        }
        running_ = false;

        if(std::this_thread::get_id() == event_thread_.get_id()){
            event_base_loopbreak(event_base_);
        } else{
            //添加一个中断事件 todo
            BreakEventKey *breakEventKey = new BreakEventKey();
            event *ev = evtimer_new(event_base_,OnQuitEvent,breakEventKey);
            event_priority_set(ev,100);
            breakEventKey->first = this;
            breakEventKey->second = ev;
            timeval timeout = {0, 0};
            evtimer_add(ev,&timeout);
        }
    }

    void EventDispatcher::Release() {
        if(event_thread_.joinable()){
            event_thread_.join();
        }
        {
            std::lock_guard<std::mutex> lock(mutex_);
            //释放
            events_.clear();
        }

        if(event_base_){
            event_base_free(event_base_);
            event_base_ = nullptr;
        }
    }

    void EventDispatcher::Register(const std::shared_ptr<Event> &event){
        if(!running_){
            return;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        events_.push_back(event);
        event_active(notify_event_,EV_READ,0);
    }

    void EventDispatcher::UnRegister(const std::shared_ptr<Event> &event){
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = std::find(events_.begin(),events_.end(),event);
        if(iter == events_.end()){
            return;
        }else{
            events_.erase(iter);
        }
    }

    void EventDispatcher::OnQuitEvent(int sock, short what, void *arg) {

    }

    void EventDispatcher::OnLoopEvent(int sock, short what, void *arg) {
        EventDispatcher *dispatcher = reinterpret_cast<EventDispatcher*>(arg);
        while(dispatcher->IsRunning()){
            std::shared_ptr<Event> event;
            std::unique_lock<std::mutex> lck(dispatcher->mutex_);
            {
                if (dispatcher->events_.empty()) break;
                event = dispatcher->events_.front();
                dispatcher->events_.pop_front();
            }
            lck.unlock();
            dispatcher->DispatchEvent(event);
        }

    }

    void EventDispatcher::DispatchEvent(const std::shared_ptr<Event> &event) {
        auto handler = event->GetHandler();
        if(handler){
            handler->HandleEvent(event);
        }
    }
}