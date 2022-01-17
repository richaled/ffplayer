#include <log/log.h>
#include "VideoDecoder.h"


VideoDecoder::~VideoDecoder() {

}

void VideoDecoder::OnDecoderReady() {
    METHOD
    m_VideoWidth = avCodecContext_->width;
    m_VideoHeight = avCodecContext_->height;
    LOGI("width : %d, height %d",m_VideoWidth,m_VideoHeight);
    //回调ready todo
    if(videorender_){
        int dstSize[2] = {0};
        videorender_->Init(m_VideoWidth,m_VideoHeight,dstSize);
        //如果渲染到anwindow上

    } else{
        LOGE("video render is null");
    }
}

void VideoDecoder::OnDecoderDone() {

}

void VideoDecoder::onFrameAvailable(AVFrame *avFrame){
    if(!videorender_){
        return;
    }
    //get avFrameType
    LOGI("get avFrame type : %d",avFrame->format);
    NativeImage image;
    switch (avFrame->format) {
        case AV_PIX_FMT_YUV420P :
            AV_PIX_FMT_YUVJ420P:
            image.format = IMAGE_FORMAT_I420;
            image.width = avFrame->width;
            image.height = avFrame->height;
            image.pLineSize[0] = avFrame->linesize[0];
            image.pLineSize[1] = avFrame->linesize[1];
            image.pLineSize[2] = avFrame->linesize[2];
            image.ppPlane[0] = avFrame->data[0];
            image.ppPlane[1] = avFrame->data[1];
            image.ppPlane[2] = avFrame->data[2];
            break;
        case AV_PIX_FMT_NV12:
            break;

    }
    videorender_->RenderFrame(&image);

}



