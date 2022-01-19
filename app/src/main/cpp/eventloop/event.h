#pragma once

#include <stdint.h>
#include <memory>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include "any.h"

namespace test {
    class EventHandler;
    class EventDispatcher;
    class Event;

    //创建event
    std::shared_ptr<Event> NewEvent(uint32_t what, const std::shared_ptr<EventHandler> &handler, const std::shared_ptr<EventDispatcher> &dispatcher);

    class Event : public std::enable_shared_from_this<Event> {
        friend class EventDispatcher;
    public:
        Event(uint32_t what) : what_(what) {
        }

        ~Event() {
        }

        /**
         * 设置数据
         */
        template<typename T>
        std::shared_ptr<Event> SetData(const std::string &key, T &&val) {
            data_[key] = any(std::forward<T>(val));
            return shared_from_this();
        }

        /**
         * 获取数据
         */
        template<typename T>
        bool GetData(const std::string &key, T &&val){
            auto iterator = data_.find(key);
            if(iterator == data_.end()){
                return false;
            }
            val = any_cast<const T&>(iterator->second);
            return false;
        }

        std::shared_ptr<Event> SetHandler(const std::shared_ptr<EventHandler> &eventHandler) {
            eventHandler_ = eventHandler;
            return shared_from_this();
        }

        std::shared_ptr<Event> SetDispatcher(const std::shared_ptr<EventDispatcher> &eventDispatcher) {
            eventDispatcher_ = eventDispatcher;
            return shared_from_this();
        }

        void Post();

        void Cancel();

        uint32_t What() const {
            return what_;
        }

        std::shared_ptr<EventHandler> GetHandler(){
            return eventHandler_.lock();
        }

    private:
        uint32_t what_ = 0;
        std::unordered_map<std::string, any> data_;
        std::weak_ptr<EventHandler> eventHandler_;
        std::weak_ptr<EventDispatcher> eventDispatcher_;
    };

}

