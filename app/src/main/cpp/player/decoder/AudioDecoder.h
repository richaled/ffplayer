#pragma once

#include "DecoderBase.h"
#include "../render/AudioRender.h"
#include "../render/ReSample.h"

class AudioDecoder : public DecoderBase{
public:
    AudioDecoder(const std::string &url):DecoderBase(url,AVMEDIA_TYPE_AUDIO){
    };
    ~AudioDecoder();

    void SetRender(const std::shared_ptr<AudioRender> &audioRender){
        audioRender_ = audioRender;
    }

protected:
    void OnDecoderReady() override ;
    void OnDecoderDone() override;
    void onFrameAvailable(AVFrame *avFrame) override ;

private:

    std::shared_ptr<AudioRender> audioRender_;
    std::shared_ptr<ReSample> reSample_;
};


