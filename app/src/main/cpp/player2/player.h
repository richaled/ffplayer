//
// Created by richard on 2022/1/18.
//
#pragma once

#include <eventloop/event.h>
#include <eventloop/event_dispatcher.h>
#include <player2/opengl/opengl_render.h>
#include <player2/opengl/egl_core.h>
#include <player2/opengl/yuv_render.h>
#include <player2/opengl/frame_render.h>
#include "eventloop/event_handler.h"
#include "media_base.h"
#include "play_impl.h"

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
        kPlayAudio,
        kPlayerPrepare
    };

    class Player : public EventHandler, public std::enable_shared_from_this<Player> {
    public:
        Player();
        virtual ~Player();

        //处理接收的事件
        void HandleEvent(const std::shared_ptr<Event> &event) override;

        void SetMediaSources(const MediaInfo &mediaInfo);

        void AddMediaClip(const MediaClip &mediaClip);

        /**
         * prepare options
         */
        void Prepare(const Options &options);

        bool IsPrepare() const {
            return isPrepare_;
        };

        int Play();

        void Resume();

        void Stop();

        void SeekTo(double timestampMs);

        void Release();

        void Reset();

        int Start();

        void CreateSurfaceWindow(void *window);
        void DestorySurfaceWindow();
        void SurfaceWindowSizeChange(const int width, const int height);

    protected:
        //window create, 不同平台各自实现
        virtual EGLSurface GetPlatformSurface(void *window);

        virtual void ReleaseSurface(void *window);

    private:
        void OnPrepare(const std::shared_ptr<Event> &event);
        void OnCreateWindow(const std::shared_ptr<Event> &event);
        void OnDestoryWindow();
        void OnPlayStart(const std::shared_ptr<Event> &event);
        void OnStop();
        bool CreateGL();
        void OnRenderVideoFrame();
        int DrawFrame();
        void CreateFrameBufferAndRender();
        void ReleaseFrame();
        void Draw(int textureId);
    protected:
        bool isPrepare_ = false;
        MediaInfo mediaInfo_;
        std::shared_ptr<EglCore> eglCore_;
        std::shared_ptr<OpenglRender> videoRender_;
        std::shared_ptr<PlayImpl> playImpl_;
        EGLSurface renderSurface_;
    private:
        std::shared_ptr<EventDispatcher> dispatcher_;
        PlayerState playerState_ = PlayerState::kNone; //播放器状态
        MediaClip currentClip_;
        bool windowCreated_ = false;
        int frameWidth_,frameHeight_;
        int surfaceWidth_,surfaceHeight_;
        std::shared_ptr<YuvRender> yuvRender_;
        std::shared_ptr<FrameBufferRender> frameBufferRender_;
        GLuint drawTextureId_;
        void *window_ = nullptr;
        Options options_;

        std::mutex mutex_;
        std::condition_variable conditionVariable_;
        GLfloat *vertexCoordinate = nullptr;

    };

}

