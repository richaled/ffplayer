//
// Created by richard on 2022/1/18.
//
#pragma once

#include <eventloop/event.h>
#include <eventloop/event_dispatcher.h>
#include "eventloop/event_handler.h"

using namespace test;
namespace player {

    enum PlayerState {
        kNone,
        kStart,
        kResume,
        kPause,
        kStop
    };

    enum PlayerMessage {
        kEGLCreate = 0,
        kRenderVideoFrame,
        kRenderImageFrame,
        kRenderSeek,
        kRenderSeekRenderFrame,
        kEGLDestroy,
        kEGLWindowCreate,
        kEGLWindowDestroy,
        kSurfaceChanged,
        kPlayerPreLoad,
        kPlayerPrepared,
        kPlayerStart,
        kPlayerResume,
        kPlayerPause,
        kPlayerStop,
        kPlayerComplete,
        kPlayerRelease,
        kPlayAudio
    };

    class Player : public EventHandler, public std::enable_shared_from_this<Player> {
    public:
        Player();
        virtual ~Player();

        void Start();

        //处理接收的事件
        void HandleEvent(const std::shared_ptr<Event> &event) override;

    private:
        std::shared_ptr<EventDispatcher> dispatcher_;
        PlayerState playerState_ = PlayerState::kNone; //播放器状态
    };

}

