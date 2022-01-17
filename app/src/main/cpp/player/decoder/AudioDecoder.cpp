#include <log/log.h>
#include "AudioDecoder.h"

AudioDecoder::~AudioDecoder() {

}

void AudioDecoder::OnDecoderReady() {
    if(audioRender_){
        audioRender_->Init();
    }
    if(!reSample_){
        reSample_ = std::make_shared<ReSample>();
    }
    reSample_->Init(avCodecContext_);
}

void AudioDecoder::OnDecoderDone() {
    if(audioRender_){
        audioRender_->UnInit();
    }
    if(reSample_){
        reSample_->UnInit();
    }

}

void AudioDecoder::onFrameAvailable(AVFrame *avFrame) {
    //对音频重采样
    if (audioRender_ && avFrame->pts > 0){
//        LOGI("get audio frame : %ld,size : %d" ,avFrame->pts,avFrame->pkt_size);
        uint8_t *buffer;
        int size = 0;
        reSample_->ResampleFrame(avFrame,&buffer,&size);
        audioRender_->RenderFrame(buffer,size);
    }
}