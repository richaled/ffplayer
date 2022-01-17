//
// Created by richard on 2022/1/17.
//
#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/frame.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>

}

class ReSample {
public:
    ReSample(){};
    ~ReSample(){};
    void Init(AVCodecContext *codeCtx);
    void ResampleFrame(AVFrame *frame,uint8_t **outBuffer,int *outSize);
    void UnInit();
private:
    SwrContext *swrContext = nullptr;
    int nbSamples_ = 0;
    int dstFrameDataSize_ = 0;
    uint8_t *buffer_;

};


