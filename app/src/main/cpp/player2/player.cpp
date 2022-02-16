#include <log/log.h>
#include <vector>
#include <player2/opengl/gl_info.h>
#include <unistd.h>
#include <glm/ext.hpp>
#include "player.h"
#include "play_impl.h"
#include "rotate_coordinate.h"

// 30 fps
#define WAIT_FRAME_SLEEP_US 33333

namespace player {

    Player::Player() {
        if (!dispatcher_) {
            dispatcher_ = std::make_shared<EventDispatcher>();
        }
//        LOGI("dispatcher start");
        if (!dispatcher_->IsRunning()) {
            dispatcher_->Run();
        }
//        LOGI("dispatcher end");
    }

    Player::~Player(){
    }

    void Player::SetMediaSources(const MediaInfo &mediaInfo) {
        mediaInfo_ = mediaInfo;
    }

    void Player::AddMediaClip(const MediaClip &mediaClip) {
        mediaInfo_.clips.push_back(mediaClip);
    }

    void Player::Prepare(const Options &options) {
        //判断当前状态
        if (playerState_ >= PlayerState::kStart) {
            return;
        }
        options_ = options;
        NewEvent(kPlayerPrepare, shared_from_this(), dispatcher_)
                ->SetData("kPlayerPrepare", 0)
                ->Post();
    }

    void Player::HandleEvent(const std::shared_ptr<Event> &event) {
        uint32_t kEvent = event->What();
        switch (kEvent) {
            case kPlayerPrepare: {
                OnPrepare(event);
                break;
            }
            case kPlayerStart: {
                OnPlayStart(event);
                break;
            }
            case kEGLWindowCreate: {
                OnCreateWindow(event);
                break;
            }
            case kEGLWindowDestroy: {
                OnDestoryWindow();
                break;
            }
            case kRenderVideoFrame: {
                OnRenderVideoFrame();
                break;
            }
        }
    }

    bool Player::CreateGL() {
//        LOGI("create opengl");
        //创建opengl环境
        eglCore_ = std::make_shared<EglCore>();
        return eglCore_->Init();
    }

    void Player::OnPrepare(const std::shared_ptr<Event> &event) {

//        std::unique_lock<std::mutex> lock(mutex_);
        int clipIndex = -1;
        event->GetDataCopy("kPlayerPrepare", clipIndex);
//        LOGI("current clipIndex : %d", clipIndex);
        if (clipIndex < 0) {
            LOGE("media clip is invalid");
//            lock.unlock();
            return;
        }
        if (!playImpl_) {
            playImpl_ = std::make_shared<PlayImpl>();
        }

        //如果设置了播放clip
        if (!mediaInfo_.IsEmpty()) {
            currentClip_ = mediaInfo_.clips[clipIndex];

            int ret = playImpl_->Init(currentClip_.file_name, options_);
            //notify prepare callback
            if(ret != 0){
                LOGE("prepare init fail");
            }
//            usleep(WAIT_FRAME_SLEEP_US * 100);
            isPrepare_ = true;
        } else {
//            LOGI("media info is empty");
        }
//        lock.unlock();
//        conditionVariable_.notify_all();
    }

    int Player::InitRender() {
        bool ret = CreateGL();
        if (!ret) {
            LOGE("create egl core error !!!");
            //callback todo
        }
        videoRender_ = std::make_shared<OpenglRender>(0, 0, DEFAULT_VERTEX_SHADER,
                                                      DEFAULT_FRAGMENT_SHADER);
        return 0;
    }

    void Player::CreateSurfaceWindow(void *window) {
        NewEvent(kEGLWindowCreate, shared_from_this(), dispatcher_)
                ->SetData("kEGLWindowCreate", window)
                ->Post();
    }

    void Player::DestorySurfaceWindow() {
        NewEvent(kEGLWindowDestroy, shared_from_this(), dispatcher_)
                ->Post();
    }

    void Player::SurfaceWindowSizeChange(const int width, const int height) {
//        METHOD
        surfaceWidth_ = width;
        surfaceHeight_ = height;
        if(videoRender_ != nullptr){
        }
    }

    void Player::OnDestoryWindow() {
        ReleaseSurface(window_);
        window_ = nullptr;
        surfaceWidth_ = 0;
        surfaceHeight_ = 0;
    }

    void Player::OnCreateWindow(const std::shared_ptr<Event> &event) {
//        THREAD_ID
        void *window;
        event->GetDataCopy("kEGLWindowCreate", window);
        window_ = window;
        bool ret = CreateGL();
        if (!ret) {
            LOGE("create gl core fail");
            return;
        }
        renderSurface_ = GetPlatformSurface(window);
        if(renderSurface_ == nullptr){
            LOGE("render surface is null");
            return;
        }
        if (renderSurface_ != nullptr && renderSurface_ != EGL_NO_SURFACE) {
            ret = eglCore_->MakeCurrent(renderSurface_);
            if (!ret) {
                LOGE("egl make current fail");
                return;
            }
        }
        windowCreated_ = true;
        //create render
        if (!videoRender_) {
            videoRender_ = std::make_shared<OpenglRender>(0, 0, DEFAULT_VERTEX_SHADER,
                                                          DEFAULT_FRAGMENT_SHADER);
        }
        //create textures

        //prepare
        if(!mediaInfo_.IsEmpty()){
            NewEvent(kPlayerPrepare, shared_from_this(), dispatcher_)
                    ->SetData("kPlayerPrepare", 0)
                    ->Post();
        }
    }

    int Player::Start() {
        if (!windowCreated_) {
            LOGE("window not created !!");
            return -1;
        }
        if(!IsPrepare()){
            NewEvent(kPlayerPrepare, shared_from_this(), dispatcher_)
                    ->SetData("kPlayerPrepare", 0)
                    ->Post();
        }
        NewEvent(kPlayerStart, shared_from_this(), dispatcher_)
                ->Post();
        return 0;
    }

    void Player::OnPlayStart(const std::shared_ptr<Event> &event) {
        if(!playImpl_){
            LOGE("media clip is invalid");
            return;
        }
//        std::unique_lock<std::mutex> lock(mutex_);
//        conditionVariable_.wait(lock, [this](){ return IsPrepare(); });
//        lock.unlock();
//        THREAD_ID
        LOGI("play start");
        playImpl_->Start();
        //渲染每一帧
        NewEvent(kRenderVideoFrame, shared_from_this(), dispatcher_)
                ->Post();
    }

    void Player::OnRenderVideoFrame() {

        if (playImpl_ == nullptr) {
            //绘制

        }
        if (playImpl_->GetPlayStaus() == PlayStatus::PLAYING) {
            //继续绘制还是等待
            int ret = DrawFrame();
            if (ret == 0) {
                //do nothing
                NewEvent(kRenderVideoFrame, shared_from_this(), dispatcher_)
                        ->Post();
            } else if (ret == -1) {
//                LOGI("render video frame delay");
                usleep(WAIT_FRAME_SLEEP_US);
                NewEvent(kRenderVideoFrame, shared_from_this(), dispatcher_)
                        ->Post();
            } else if (ret == -2) {

            }
        } else if (playImpl_->GetPlayStaus() == IDEL) {
            //等待
        } else if (playImpl_->GetPlayStaus() == BUFFER_EMPTY) {

        }
    }

    int Player::DrawFrame() {
//        METHOD
        if(playImpl_ == nullptr){
            LOGE("play impl is null !!!");
            return -2;
        }
//        LOGI("draw frame current time %lld",GetSysCurrentTime());
//        usleep(WAIT_FRAME_SLEEP_US);
//        return 0;
        if (!eglCore_->MakeCurrent(renderSurface_)) {
            LOGE("MakeCurrent error: %d", eglGetError());
            return -1;
        }
        if (playImpl_->videoFrame_ == nullptr) {
            //从队列中获取
            playImpl_->videoFrame_ = playImpl_->GetFromFrameQueue();
        }
        if(playImpl_->videoFrame_ == nullptr){
//            LOGI("get queue frame is null");
            return -1;
        }
        int64_t videoFramePts = playImpl_->GetVideoFramePts();
//        LOGI("videoFramePts : %ld， pts : %ld" ,videoFramePts, playImpl_->videoFrame_->pts);
        usleep(WAIT_FRAME_SLEEP_US);
        CreateFrameBufferAndRender();
        ReleaseFrame();
        return 0;

        //
//        int64_t videoFramePts = playImpl_->GetVideoFramePts();
        //如果有音频，就以音频为主时钟
        int64_t masterClock;
//        if (playImpl_->HasAudio()) {
//            masterClock = 0;// todo
//        } else {
            masterClock = clock_get(playImpl_->videoClock_);
//        }
//        LOGI("videoFramePts : %ld， pts : %ld,masterClock = %ld" ,videoFramePts, playImpl_->videoFrame_->pts,masterClock);

        //做音视频同步
        int64_t diff = videoFramePts - masterClock;
        int64_t min = 25000;
        int64_t max = 1000000;
        if (diff > -min && diff < min) {
            //create framebuffer and render
            CreateFrameBufferAndRender();
            clock_set(playImpl_->videoClock_, videoFramePts);
        } else if (diff >= min && diff < max) {
            usleep(static_cast<useconds_t>(diff - min));
            CreateFrameBufferAndRender();
            clock_set(playImpl_->videoClock_, videoFramePts - (diff - min));
        } else {
            //release frame
            ReleaseFrame();
        }
        return 0;
    }

    void Player::CreateFrameBufferAndRender() {
        if (playImpl_->videoFrame_ == nullptr) {
            return;
        }
        int width = std::min(playImpl_->videoFrame_->linesize[0], playImpl_->videoFrame_->width);
        int height = playImpl_->videoFrame_->height;
        if (frameWidth_ != width || frameHeight_ != height) {
            frameWidth_ = width;
            frameHeight_ = height;

            if (playImpl_->IsHardWare()) {

            } else {
                //如果是软解，直接使用yuv渲染
                yuvRender_ = std::make_shared<YuvRender>();
            }
            if (surfaceWidth_ != 0 && surfaceHeight_ != 0) {

            }
        }

        if (playImpl_->IsHardWare()) {

        } else {
            auto texture_coordinate = rotate_soft_decode_media_encode_coordinate(0);
            auto ratio = playImpl_->videoFrame_->width * 1.F / playImpl_->videoFrame_->linesize[0];
            glm::mat4 scale_matrix = glm::mat4(ratio);
            drawTextureId_ = yuvRender_->DrawFrame(playImpl_->videoFrame_,glm::value_ptr(scale_matrix),
                                                   DEFAULT_VERTEX_COORDINATE, texture_coordinate);
            auto currentTime = playImpl_->GetVideoFramePts();
//            LOGI("render currentTime : %ld , texture : %d" ,currentTime,drawTextureId_);
        }

        //释放frame
//        ReleaseFrame();
        if (drawTextureId_ == -1) {
            return;
        }
        //如果有图像处理
        Draw(drawTextureId_);
    }

    void Player::ReleaseFrame() {
        UnrefFrameFromPool(playImpl_->videoFramePool_,playImpl_->videoFrame_);
        playImpl_->videoFrame_ = nullptr;
    }

    void Player::Draw(int textureId) {
//        METHOD
        if (videoRender_ == nullptr) {
            auto mThreadId_ = std::this_thread::get_id();
            LOGE("video render null !! %d",mThreadId_);
            return;
        }
        if(eglCore_ == nullptr){
            LOGE("egl core null !!");
            return;
        }
        if (renderSurface_ == nullptr || renderSurface_ == EGL_NO_SURFACE) {
            LOGE("render surface invalid !!");
            return;
        }
        //回调上层, 可以拿到外部处理的纹理进行处理
//        int
//        LOGI("process image");
        videoRender_->ProcessImage(textureId,DEFAULT_VERTEX_COORDINATE,DEFAULT_TEXTURE_COORDINATE);
        if (!eglCore_->SwapBuffers(renderSurface_)) {
            LOGE("eglSwapBuffers error: %d", eglGetError());
        }
    }

    EGLSurface Player::GetPlatformSurface(void *window) {
        return nullptr;
    }
    void Player::ReleaseSurface(void *window) {

    }
}