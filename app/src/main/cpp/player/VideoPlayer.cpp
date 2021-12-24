#include "VideoPlayer.h"

VideoPlayer::~VideoPlayer() {

}

void VideoPlayer::Init(const std::string &url, const std::shared_ptr<VideoRender> &render) {
    //创建解码和渲染的对象
    videoDecoder_ = std::make_shared<VideoDecoder>(url);
//    videoDecoder_.reset(OpenGlVideoRender::GetInstance());

    //解码器设置render
    videoDecoder_->SetRenderer(render);
}

void VideoPlayer::UnInit() {
//    if(videoRender_){
//        videoRender_->UnInit();
//        videoRender_ = nullptr;
//    }
    if(videoDecoder_){

    }
}


void VideoPlayer::Play() {
    if (videoDecoder_){
        videoDecoder_->Start();
    }
}

void VideoPlayer::Stop() {
    if(videoDecoder_){
        videoDecoder_->Stop();
    }
}

void VideoPlayer::Pause() {
    if(videoDecoder_){
        videoDecoder_->Pause();
    }
}

void VideoPlayer::SeekTo(float position) {
    if(videoDecoder_){
        videoDecoder_->SeekTo(position);
    }
}

float VideoPlayer::GetDuration() {
    return videoDecoder_->GetDuration();
}

//void VideoPlayer::OnSurfaceCreated() {
//    videoRender_->OnSurfaceCreated();
//}
//
//void VideoPlayer::OnSurfaceChanged(int width, int height) {
//    videoRender_->OnSurfaceChanged(width,height);
//}
//
//void VideoPlayer::OnDrawFrame() {
//    videoRender_->OnDrawFrame();
//}
