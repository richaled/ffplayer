#pragma once

#include <mutex>
#include "thread"
#include "../render/VideoRender.h"
#include "DecoderBase.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
#include <libavcodec/jni.h>
}

class VideoDecoder : public DecoderBase{
public:
    VideoDecoder(const std::string &url):DecoderBase(url,AVMEDIA_TYPE_VIDEO){
    };

    ~VideoDecoder();

    void SetRenderer(const std::shared_ptr<VideoRender> &render){
        videorender_ = render;
    };

protected:
    void OnDecoderReady() override ;
    void OnDecoderDone() override;
    void onFrameAvailable(AVFrame *avFrame) override ;
private:

private:
    int m_VideoWidth = 0;
    int m_VideoHeight = 0;
    int m_RenderWidth = 0;
    int m_RenderHeight = 0;
    std::shared_ptr<VideoRender> videorender_;
};


