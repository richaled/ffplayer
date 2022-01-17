#include "ReSample.h"


// 音频编码采样率
static const int AUDIO_DST_SAMPLE_RATE = 44100;
// 音频编码通道数
static const int AUDIO_DST_CHANNEL_COUNTS = 2;
// 音频编码声道格式
static const uint64_t AUDIO_DST_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
// 音频编码比特率
static const int AUDIO_DST_BIT_RATE = 64000;
// ACC音频一帧采样数
static const int ACC_NB_SAMPLES = 1024;
static const AVSampleFormat DST_SAMPLT_FORMAT = AV_SAMPLE_FMT_S16;


void ReSample::Init(AVCodecContext *codeCtx) {

    swrContext = swr_alloc();

    av_opt_set_int(swrContext, "in_channel_layout", codeCtx->channel_layout, 0);
    av_opt_set_int(swrContext, "out_channel_layout", AUDIO_DST_CHANNEL_LAYOUT, 0);

    av_opt_set_int(swrContext, "in_sample_rate", codeCtx->sample_rate, 0);
    av_opt_set_int(swrContext, "out_sample_rate", AUDIO_DST_SAMPLE_RATE, 0);

    av_opt_set_sample_fmt(swrContext, "in_sample_fmt", codeCtx->sample_fmt, 0);
    av_opt_set_sample_fmt(swrContext, "out_sample_fmt", DST_SAMPLT_FORMAT,  0);

    swr_init(swrContext);
    //

    nbSamples_ = av_rescale_rnd(ACC_NB_SAMPLES,AUDIO_DST_SAMPLE_RATE,codeCtx->sample_rate,AV_ROUND_UP);
    dstFrameDataSize_ = av_samples_get_buffer_size(nullptr,AUDIO_DST_CHANNEL_COUNTS,nbSamples_,DST_SAMPLT_FORMAT,1);

    //outbuffer
    buffer_ = (uint8_t*)malloc(dstFrameDataSize_);
}

void ReSample::ResampleFrame(AVFrame *frame,uint8_t **outBuffer,int *outSize) {
    if(!swrContext){
        return;
    }
    int result = swr_convert(swrContext,&buffer_,dstFrameDataSize_ /2 , (const uint8_t **) frame->data,frame->nb_samples);
    if(result > 0){
        *outBuffer = buffer_;
        *outSize = dstFrameDataSize_;
    }
}

void ReSample::UnInit() {
    if(buffer_){
        free(buffer_);
        buffer_ = nullptr;
    }
    if(swrContext){
        swr_free(&swrContext);
        swrContext = nullptr;
    }
}