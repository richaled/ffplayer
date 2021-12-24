#pragma once

#include <mutex>
#include "Decoder.h"
#include "thread"
//#include "../player.render/VideoRender.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
#include <libavcodec/jni.h>
}

#define DELAY_THRESHOLD 10


class VideoDecoder : public Decoder{
public:
    VideoDecoder(const std::string &url):url_(url){};

    ~VideoDecoder(){};

    void Start() override ;

    void Pause() override;

    void Stop() override;

    void SeekTo(float position) override;

    float GetDuration() override;

//    void SetRenderer(const std::shared_ptr<VideoRender> &player.render){
//        videorender_ = std::move(player.render);
//    };

protected:
    void OnDecoderReady();
    void OnDecoderDone();

private:
    int InitFFDecoder();

    void UnInitDecoder();

    //启动解码线程
    void StartDecodingThread();

    //音视频解码循环
    void DecodingLoop();

    //更新显示时间戳
    void UpdateTimeStamp();

    //音视频同步
    long AVSync();

    //解码一个packet编码数据
    int DecodeOnePacket();

private:
    AVFormatContext *avFormatContext_;
    AVCodecContext *avCodecContext_;
    AVCodec *avCodec_;

    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable conditionVariable_;
    std::string url_;
    AVMediaType m_MediaType = AVMEDIA_TYPE_UNKNOWN;
    int m_StreamIndex = -1;
    long duration_;
    DecoderState decoderState_;
    //播放的起始时间
    long m_StartTimeStamp = -1;
    long currentTimeStamp = -1;
    AVFrame *avFrame_;

    const AVPixelFormat DST_PIXEL_FORMAT = AV_PIX_FMT_RGBA;

    int m_VideoWidth = 0;
    int m_VideoHeight = 0;
    int m_RenderWidth = 0;
    int m_RenderHeight = 0;

//    std::shared_ptr<VideoRender> videorender_;
};


