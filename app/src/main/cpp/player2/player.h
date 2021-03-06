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
        bool IsPlaying() const {
            return playImpl_ && playImpl_->playStatus_ == PlayStatus::PLAYING;
        }

        void Resume();

        void Pause();

        void Stop();

        void SeekTo(int64_t timestampMs);

        void Release();

        void Reset();

        int Start();
        //预加载下一个片段
        void PrepredLoad();

        int64_t GetDuration() const {
            return durationMs_;
        }

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
        void OnResume();
        void OnPause();
        void OnSeek(const std::shared_ptr<Event> &event);
        void OnRenderSeekFrame();
        void OnComplete();
        void OnPreLoad(const std::shared_ptr<Event> &event);
        bool CreateGL();
        void OnRenderVideoFrame();
        int DrawVideoFramePrepared();
        int DrawFrame();
        void CreateFrameBufferAndRender();
        void ReleaseFrame();
        void Draw(int textureId);
        int64_t GetMasterClock();
        int64_t ComputeTargetDelay(int64_t  delay);
        std::shared_ptr<PlayImpl> CreatePlayImpl(const MediaClip &mediaClip);
        bool VideoDisplay();
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
        MediaClip preLoadClip_;
        bool windowCreated_ = false;
        int frameWidth_,frameHeight_;
        int surfaceWidth_,surfaceHeight_;
        std::shared_ptr<YuvRender> yuvRender_;
        std::shared_ptr<FrameBufferRender> frameBufferRender_;
        GLuint drawTextureId_;
        void *window_ = nullptr;
        Options options_;
        int syncStrategy_ = SyncStrategy::EXTERNAL;

        std::mutex mutex_;
        std::condition_variable conditionVariable_;
        GLfloat *vertexCoordinate = nullptr;
        GLfloat *textureCoordinate = nullptr;

        int seekIndex_ = 0;
        int64_t durationMs_ = 0;
        int64_t lastPts_ = 0;
        int64_t frameTime_ = -1;//当前帧显示时间
        int remaingTime_ = 0;
        int clipIndex_ = 0;
        std::vector<std::shared_ptr<PlayImpl>> playImpls_;
    };

}

