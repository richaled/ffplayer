#include <log/log.h>
#include <vector>
#include <player2/opengl/gl_info.h>
#include <unistd.h>
#include <glm/ext.hpp>
#include <player2/opengl/source_scale.h>
#include "player.h"
#include "play_impl.h"
#include "rotate_coordinate.h"

// 30 fps
#define WAIT_FRAME_SLEEP_US 33333

//同步阈值
#define SYNC_THRESHOLD_MIN 40000
#define SYNC_THRESHOLD_MAX 100000
//一帧显示的最大时长 10s
#define MAX_FRAME_DURATION 10000000
//
#define SYNC_FRAMEDUP_THRESHOLD 100000


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
        durationMs_ = mediaInfo_.GetDuration();
    }

    void Player::AddMediaClip(const MediaClip &mediaClip) {
        mediaInfo_.clips.push_back(mediaClip);
        durationMs_ = mediaInfo_.GetDuration();
    }

    void Player::Prepare(const Options &options) {
        //判断当前状态
        if (playerState_ >= PlayerState::kStart) {
            return;
        }
        options_ = options;
        syncStrategy_ = OptionsGet(options,test::OptionKey::kSyncStrategy, SyncStrategy::AUDIO);
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
            case kPlayerStop:{
                OnStop();
                break;
            }
            case kPlayerResume:{
                OnResume();
                break;
            }
            case kPlayerPause:{
                OnPause();
                break;
            }
            case kRenderSeek:{
                OnSeek(event);
                break;
            }
            case kRenderSeekRenderFrame:{
                OnRenderSeekFrame();
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

            int ret = playImpl_->Init(currentClip_, options_);
            //notify prepare callback
            if(ret != 0){
                LOGE("prepare init fail");
            }

            //初始化

//            usleep(WAIT_FRAME_SLEEP_US * 100);
            isPrepare_ = true;
        } else {
//            LOGI("media info is empty");
        }
//        lock.unlock();
//        conditionVariable_.notify_all();
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
        windowCreated_ = false;
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
        vertexCoordinate = GetDefaultVertexCoord();
        textureCoordinate = GetDefalutTextureCoord();
        //create render
        if (!videoRender_) {
            videoRender_ = std::make_shared<OpenglRender>(surfaceWidth_, surfaceHeight_, DEFAULT_VERTEX_SHADER,
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
        std::unique_lock<std::mutex> lock(mutex_);
        conditionVariable_.wait(lock, [this](){ return IsPrepare(); });
        LOGI("play start");
        lock.unlock();
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

    int Player::DrawVideoFramePrepared() {
        if(playImpl_ == nullptr){
            LOGE("play impl is null !!!");
            return -1;
        }
        if (!eglCore_->MakeCurrent(renderSurface_)) {
            LOGE("MakeCurrent error: %d", eglGetError());
            return -1;
        }
        //如果是文件尾，并且缓存中没有frame，返回-2

        if (playImpl_->videoFrame_ == nullptr) {
            //从队列中获取
            playImpl_->videoFrame_ = playImpl_->GetFromFrameQueue();
        }

        if(playImpl_->videoFrame_ == nullptr){
//            LOGI("get queue frame is null");
            return -1;
        }
        return 0;
    }

    int Player::DrawFrame() {
//        METHOD
        int ret = DrawVideoFramePrepared();
        if(ret != 0){
            return ret;
        }

        int64_t videoFramePts = playImpl_->GetVideoFramePts();
//        LOGI("videoFramePts : %ld， pts : %ld" ,videoFramePts, playImpl_->videoFrame_->pts);
//        usleep(WAIT_FRAME_SLEEP_US);
//        CreateFrameBufferAndRender();
//        ReleaseFrame();
//        return 0;

        //得到当前时间
        int64_t time = clock_get_current_time();
        LOGI("videoFramePts : %ld" ,videoFramePts);
        if(frameTime_ == -1){
            frameTime_ = time;
        }

        //计算前面一帧显示的时长
        int64_t lastDuration = videoFramePts - lastPts_;
        lastPts_ = videoFramePts;
        //计算当前帧显示的时长
        int64_t dealy = ComputeTargetDelay(lastDuration);
        //得到当前的系统时间
        LOGI("delay : %ld，currentTime ：%ld，lastDuration %ld",dealy,time,lastDuration);
        if(time < frameTime_ + dealy){
            remaingTime_ = FFMIN(frameTime_ + dealy - time, remaingTime_);
            LOGI("remaining_time : %ld",remaingTime_);
            usleep(remaingTime_);
            //直接显示
            LOGI("show frame");
            CreateFrameBufferAndRender();
        }else{
            frameTime_ += dealy;
            LOGI("frameTime_ : %ld",frameTime_);
            if (dealy > 0 && time - frameTime_ > SYNC_THRESHOLD_MAX){
                frameTime_ = time;
            }
            //更新视频时钟
            clock_set(playImpl_->videoClock_, videoFramePts);
            sync_clock_to_slave(playImpl_->extClock_,playImpl_->videoClock_);

            //获取下一帧

            //丢弃当前帧
            LOGE("discard current frame !!!");
//            DrawFrame();
        }
        ReleaseFrame();
        //做音视频同步
       /* int64_t diff = videoFramePts - masterClock;
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
        }*/
        return 0;
    }

    int64_t Player::GetMasterClock() {
        int64_t masterClock = 0;
        switch (syncStrategy_) {
            case SyncStrategy::AUDIO:{
                if(!playImpl_->HasAudio()){
                    masterClock = clock_get(playImpl_->extClock_);
                } else{
                    masterClock = clock_get(playImpl_->videoClock_);
                }
                break;
            }
            case SyncStrategy::EXTERNAL:{
                masterClock = clock_get(playImpl_->extClock_);
                break;
            }
        }
        return masterClock;
    }

    int64_t Player::ComputeTargetDelay(int64_t  delay) {
        int64_t syncThreshold, diff = 0;
        diff = clock_get(playImpl_->videoClock_) - GetMasterClock();
        LOGI("diff = %ld",diff);
        syncThreshold = FFMAX(SYNC_THRESHOLD_MIN,FFMIN(SYNC_THRESHOLD_MAX,delay));
        //如果时钟之差是有效值，并且
        if(abs(diff) < MAX_FRAME_DURATION){
            delay = FFMAX(0,delay + diff);
        }else if(diff >= syncThreshold && delay > SYNC_FRAMEDUP_THRESHOLD){
            delay += diff;
        } else if(diff >= syncThreshold){
            delay = 2 * delay;
        }
        return delay;
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
                //计算顶点
                vertexCoordinate = GetScaleVertexCoord(frameWidth_,frameHeight_,surfaceWidth_,surfaceHeight_);
            }
        }

        if (playImpl_->IsHardWare()) {

        } else {
            auto texture_coordinate = rotate_soft_decode_media_encode_coordinate(playImpl_->GetFrameRotate());
            auto ratio = playImpl_->videoFrame_->width * 1.F / playImpl_->videoFrame_->linesize[0];
            glm::mat4 scale_matrix = glm::mat4(ratio);
            drawTextureId_ = yuvRender_->DrawFrame(playImpl_->videoFrame_,glm::value_ptr(scale_matrix),
                                                   DEFAULT_VERTEX_COORDINATE, texture_coordinate);
            auto currentTime = playImpl_->GetVideoFramePts();
//            LOGI("render currentTime : %ld , texture : %d" ,currentTime,drawTextureId_);
        }

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
        videoRender_->ProcessImage(textureId,vertexCoordinate,DEFAULT_TEXTURE_COORDINATE);
        if (!eglCore_->SwapBuffers(renderSurface_)) {
            LOGE("eglSwapBuffers error: %d", eglGetError());
        }
    }

    void Player::Stop() {
        NewEvent(kPlayerStop, shared_from_this(), dispatcher_)
                ->Post();
    }

    void Player::OnStop() {
        if(playImpl_){
            playImpl_->Stop();
        }
        frameWidth_ = 0;
        frameHeight_ = 0;
        playerState_ = kStop;
    }

    void Player::Resume() {
        NewEvent(kPlayerResume, shared_from_this(), dispatcher_)
                ->Post();
    }

    void Player::OnResume() {
        if(!currentClip_.IsValid()){
            LOGE("resume clip is invalid !!!");
            return;
        }
        if(playImpl_->playStatus_ == PlayStatus::PAUSED || playImpl_->playStatus_ == PlayStatus::SEEKING){
            playImpl_->Resume();
        }
        playerState_ = kResume;
        NewEvent(kRenderVideoFrame, shared_from_this(), dispatcher_)
                ->Post();
    }

    void Player::Pause() {
        NewEvent(kPlayerPause, shared_from_this(), dispatcher_)
                ->Post();
    }

    void Player::OnPause() {
        if(playImpl_->playStatus_ == PlayStatus::PLAYING){
            playImpl_->Pause();
        }
        playerState_ = kPause;
    }

    void Player::SeekTo(int64_t timestampMs) {
        dispatcher_->Flush();
        //根据时间戳找到对应的clip
        auto clips = mediaInfo_.clips;
        int index = mediaInfo_.GetClipTime(timestampMs);
        if(index >= 0){
            auto mediaClip = mediaInfo_.clips.at(index);
            int64_t seekTime = mediaInfo_.GetVideoTime(index, 0);

            LOGI("seek clips : %s, index : %d, seekTime %ld",mediaClip.ToString().c_str(),index,seekTime);

            NewEvent(kRenderSeek, shared_from_this(), dispatcher_)
                    ->SetData("kRenderSeekClip", mediaClip)
                    ->SetData("kRenderSeekTime", (timestampMs - seekTime))
                    ->SetData("kRenderSeekIndex", index)
                    ->Post();
        } else{
            LOGE("seek clip index is -1");
        }
    }

    void Player::OnSeek(const std::shared_ptr<Event> &event) {
        int clipIndex = -1;
        MediaClip mediaClip;
        int64_t seekTime;
        event->GetDataCopy("kRenderSeekIndex", clipIndex);
        event->GetDataCopy("kRenderSeekClip", mediaClip);
        event->GetDataCopy("kRenderSeekTime", seekTime);
        LOGI("onseek clip : %s",mediaClip.ToString().c_str());
        if(!mediaClip.IsValid()){
            return;
        }
        currentClip_ = mediaClip;
        if(clipIndex != seekIndex_){
            //reset
        }
        seekIndex_ = clipIndex;
        bool seekAble = playImpl_->SeekTo(seekTime);
        LOGI("seekable : %d",seekAble);
        if(seekAble){
            NewEvent(kRenderSeekRenderFrame, shared_from_this(), dispatcher_)
                    ->Post();
        }
    }

    void Player::OnRenderSeekFrame() {
        if(eglCore_ == nullptr){
            return;
        }
        int ret = DrawVideoFramePrepared();
        if(ret == 0){
            CreateFrameBufferAndRender();
            ReleaseFrame();
        }else{
            //通知完成
//            NewEvent(kRenderSeekRenderFrame, shared_from_this(), dispatcher_)
//                    ->Post();
        }
    }

    EGLSurface Player::GetPlatformSurface(void *window) {
        return nullptr;
    }
    void Player::ReleaseSurface(void *window) {

    }

}