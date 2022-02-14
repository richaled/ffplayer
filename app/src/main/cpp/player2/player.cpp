#include <log/log.h>
#include <vector>
#include <player2/opengl/gl_info.h>
#include <unistd.h>
#include "player.h"
#include "play_impl.h"

// 30 fps
#define WAIT_FRAME_SLEEP_US 33333

namespace player{

    Player::Player() {
        if(!dispatcher_){
            dispatcher_ = std::make_shared<EventDispatcher>();
        }
        LOGI("dispatcher start");
        if(!dispatcher_->IsRunning()){
            dispatcher_->Run();
        }
        LOGI("dispatcher end");
    }

    Player::~Player() noexcept {

    }
    void Player::Prepare(const MediaInfo &mediaInfo){
        //判断当前状态
        if(playerState_ >= PlayerState::kStart){
            return;
        }
        mediaInfo_ = mediaInfo;
        NewEvent(kPlayerPrepare,shared_from_this(),dispatcher_)
                ->Post();
    }

    void Player::HandleEvent(const std::shared_ptr<Event> &event) {
        uint32_t kEvent = event->What();
        switch (kEvent) {
            case kPlayerPrepare:{
                LOGI("play prepare");
                OnPrepare(event);
                break;
            }
            case kPlayerStart:{
                OnPlayStart(event);
                break;
            }
            case kEGLWindowCreate:{
                OnCreateWindow(event);
                break;
            }
            case kEGLWindowDestroy:{
                OnDestoryWindow();
                break;
            }
            case kRenderVideoFrame:{
                OnRenderVideoFrame();
                break;
            }
        }
    }

    bool Player::CreateGL() {
        LOGI("create opengl");
        //创建opengl环境
        eglCore_ = std::make_shared<EglCore>();
        return eglCore_->Init();
    }

    void Player::OnPrepare(const std::shared_ptr<Event> &event) {
        LOGI("clip size : %d",mediaInfo_.clips.size());

        MediaClip mediaClip;
        event->GetDataCopy("kPlayerPrepare", mediaClip);
        if(!mediaClip.IsValid()){
            LOGE("media clip is invalid");
            return;
        }
        if(!playImpl_){
            playImpl_ = std::make_shared<PlayImpl>();
        }
        playImpl_->Init(mediaClip.file_name);
    }

    int Player::InitRender() {
        bool ret = CreateGL();
        if(!ret){
            LOGE("create egl core error !!!");
            //callback todo
        }
        videoRender_ = std::make_shared<OpenglRender>(0,0,DEFAULT_VERTEX_SHADER,DEFAULT_FRAGMENT_SHADER);
        return 0;
    }

    void Player::CreateSurfaceWindow(void *window) {
        NewEvent(kEGLWindowCreate,shared_from_this(),dispatcher_)
        ->SetData("windowCreate",window)
        ->Post();
    }

    void Player::DestorySurfaceWindow() {
        NewEvent(kEGLWindowDestroy,shared_from_this(),dispatcher_)
                ->Post();
    }

    void Player::SurfaceWindowSizeChange(const int width, const int height) {
        surfaceWidth_ = width;
        surfaceHeight_ = height;
    }

    void Player::OnDestoryWindow() {

    }

    void Player::OnCreateWindow(const std::shared_ptr<Event> &event) {
        void *window;
        event->GetDataCopy("windowCreate", window);
        bool ret = CreateGL();
        if(!ret){
            LOGE("create gl core fail");
            return;
        }
        renderSurface_ = GetPlatformSurface(window);
        if(renderSurface_ != nullptr && renderSurface_ != EGL_NO_SURFACE){
            ret = eglCore_->MakeCurrent(renderSurface_);
            if(!ret){
                LOGE("egl make current fail");
                return;
            }
        }
        windowCreated_ = true;

        //create render
        if(!videoRender_){
            videoRender_ = std::make_shared<OpenglRender>(0,0,DEFAULT_VERTEX_SHADER,DEFAULT_FRAGMENT_SHADER);
        }
        //create textures



    }

    int Player::Start(const MediaClip &mediaClip) {
        currentClip_ = mediaClip;
        if (!windowCreated_) {
            LOGE("window not created !!");
            return -1;
        }
        if(currentClip_.IsValid()){
            NewEvent(kPlayerPrepare,shared_from_this(),dispatcher_)
                    ->SetData("kPlayerPrepare",currentClip_)
                    ->Post();
            NewEvent(kPlayerStart,shared_from_this(),dispatcher_)
                    ->SetData("kPlayerStart",currentClip_)
                    ->Post();
        }
    }

    void Player::OnPlayStart(const std::shared_ptr<Event> &event) {
        MediaClip mediaClip;
        event->GetDataCopy("kPlayerStart", mediaClip);
        if(!mediaClip.IsValid()){
            LOGE("media clip is invalid");
            return;
        }

        //渲染每一帧
        NewEvent(kRenderVideoFrame,shared_from_this(),dispatcher_)
                ->Post();
    }

    void Player::OnRenderVideoFrame() {
        if(eglCore_ == nullptr){
            //wait
        }
        if(playImpl_ == nullptr){
            //绘制

        }
        if(playImpl_->GetPlayStaus() == PlayStatus::PLAYING){
            //继续绘制还是等待
            int ret = DrawFrame();
            if(ret == 0){
                //do nothing
                NewEvent(kRenderVideoFrame,shared_from_this(),dispatcher_)
                        ->Post();
            }else if(ret == -1){
                usleep(WAIT_FRAME_SLEEP_US);
                NewEvent(kRenderVideoFrame,shared_from_this(),dispatcher_)
                        ->Post();
            }else if(ret == -2){

            }
        } else if(playImpl_->GetPlayStaus() == IDEL){

        }else if(playImpl_->GetPlayStaus() == BUFFER_EMPTY){

        }
    }

    int Player::DrawFrame() {
        if(!eglCore_->MakeCurrent(renderSurface_)){
            LOGE("MakeCurrent error: %d", eglGetError());
            return -1;
        }
        if(playImpl_->videoFrame_ == nullptr){
            //从队列中获取
            playImpl_->videoFrame_ = playImpl_->GetFromFrameQueue();
        }

        //
        int64_t videoFramePts = playImpl_->GetVideoFramePts();
        //如果有音频，就以音频为主时钟
        int64_t masterClock;
        if(playImpl_->HasAudio()){
            masterClock = 0;// todo
        }else{
            masterClock = clock_get(playImpl_->videoClock_);
        }

        //做音视频同步
        int64_t diff = videoFramePts - masterClock;
        int64_t min = 25000;
        int64_t max = 1000000;
        if(diff > -min && diff < min){
            //create framebuffer and render
            CreateFrameBufferAndRender();
            clock_set(playImpl_->videoClock_, videoFramePts);
        } else if(diff >= min && diff < max){
            usleep(static_cast<useconds_t>(diff - min));
            CreateFrameBufferAndRender();
            clock_set(playImpl_->videoClock_, videoFramePts - (diff - min));
        } else{
            //release frame

        }

        return 0;
    }

    void Player::CreateFrameBufferAndRender() {
        if(playImpl_->videoFrame_ == nullptr){
            return;
        }
        int width = std::min(playImpl_->videoFrame_->linesize[0], playImpl_->videoFrame_->width);
        int height = playImpl_->videoFrame_->height;
        if(frameWidth_ != width || frameHeight_ != height){
            frameWidth_ = width;
            frameHeight_ = height;

            if(playImpl_->IsHardWare()){

            }else{
                //如果是软解，直接使用yuv渲染
                yuvRender_ = std::make_shared<YuvRender>();
            }
            if(surfaceWidth_ != 0 && surfaceHeight_ != 0){

            }
        }

        if(playImpl_->IsHardWare()){

        }else{
//            drawTextureId_ = yuvRender_->DrawFrame(playImpl_->videoFrame_,);
        }

        //释放frame
        ReleaseFrame();
        if(drawTextureId_ == -1){
            return;
        }
        //如果有图像处理
        Draw(drawTextureId_);
    }

    void Player::ReleaseFrame() {
        playImpl_->videoFrame_ = nullptr;
    }

    void Player::Draw(int textureId) {
        if(eglCore_ == nullptr){
            return;
        }
        if(videoRender_ != nullptr){
            return;
        }
        if(renderSurface_ == EGL_NO_SURFACE){
            return;
        }
        //回调上层, 可以拿到外部处理的纹理进行处理

//        videoRender_->ProcessImage(textureId,);
        if(!eglCore_->SwapBuffers(renderSurface_)){
            LOGE("eglSwapBuffers error: %d", eglGetError());
        }
    }
}