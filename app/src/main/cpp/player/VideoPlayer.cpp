#include "VideoPlayer.h"

VideoPlayer::~VideoPlayer() {

}

void VideoPlayer::Init(const std::string &url, const std::shared_ptr<VideoRender> &render) {
    //创建解码和渲染的对象
    videoDecoder_ = std::make_shared<VideoDecoder>(url);
    audioDecoder_ = std::make_shared<AudioDecoder>(url);

    auto audioRender = std::make_shared<AudioRender>();
    //解码器设置render
    videoDecoder_->SetRenderer(render);
    audioDecoder_->SetRender(audioRender);
    if(stateCallback_){
        videoDecoder_->SetStateCallback(stateCallback_);
        audioDecoder_->SetStateCallback(stateCallback_);
    }
    if(progressCallback_){
        videoDecoder_->SetProgressCallback(progressCallback_);
        audioDecoder_->SetProgressCallback(progressCallback_);
    }
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
    if(audioDecoder_){
        audioDecoder_->Start();
    }
}

void VideoPlayer::Stop() {
    if(videoDecoder_){
        videoDecoder_->Stop();
    }
    if(audioDecoder_){
        audioDecoder_->Stop();
    }
}

void VideoPlayer::Pause() {
    if(videoDecoder_){
        videoDecoder_->Pause();
    }
    if(audioDecoder_){
        audioDecoder_->Pause();
    }
}

void VideoPlayer::SeekTo(float position) {
    if(videoDecoder_){
        videoDecoder_->SeekTo(position);
    }
    if(audioDecoder_){
        audioDecoder_->SeekTo(position);
    }
}

float VideoPlayer::GetDuration() {
    return videoDecoder_->GetDuration();
}

void VideoPlayer::SetStateCallback(const StateCallback &stateCallback) {
    if(videoDecoder_){
        videoDecoder_->SetStateCallback(stateCallback);
    }
    if(audioDecoder_){
        audioDecoder_->SetStateCallback(stateCallback);
    }
    stateCallback_ = stateCallback;
}

void VideoPlayer::SetProgressCallback(const ProgressCallback &progressCallback) {
    if(videoDecoder_){
        videoDecoder_->SetProgressCallback(progressCallback);
    }
    if(audioDecoder_){
        audioDecoder_->SetProgressCallback(progressCallback);
    }
    progressCallback_ = progressCallback;
}
