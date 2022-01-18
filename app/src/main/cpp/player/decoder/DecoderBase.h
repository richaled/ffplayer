#pragma once

#include "Decoder.h"
#include "thread"

#define DELAY_THRESHOLD 50

/**
 * 解码的基类
 */
class DecoderBase : public Decoder {


public:
    DecoderBase(const std::string &url, const AVMediaType &mediaType):url_(url),mediaType_(mediaType){};

    virtual ~DecoderBase();

    void Start() override;

    void Pause() override;

    void Stop() override;

    void SeekTo(float position) override;

    float GetDuration() override;

    void SetStateCallback(const StateCallback &stateCallback){
        stateCallback_ = stateCallback;
    }
    void SetProgressCallback(const ProgressCallback &progressCallback){
        progressCallback_ = progressCallback;
    }

protected:
    virtual void OnDecoderReady() = 0;
    virtual void OnDecoderDone() = 0;
    virtual void onFrameAvailable(AVFrame *avFrame) = 0;
private:
/**
 * 启动解码线程
 */
    void StartDecodingThread();
    /**
     * 初始化ffmpeg解码器
     */
    int InitFFDecoder();

    /**
     * 销毁ffmeg资源
     */
    void UnInitDecoder();

    /**
     * 音视频解码循环
     */
    void DecodingLoop();

    int DecodeOnePacket();

    //更新显示时间戳
    void UpdateTimeStamp();

    //音视频同步
    long AVSync();

protected:
    std::string url_;
    AVMediaType mediaType_ = AVMEDIA_TYPE_UNKNOWN;

    std::thread thread_;
    AVFormatContext *avFormatContext_;
    AVCodecContext *avCodecContext_;
    AVCodec *avCodec_;


    int streamIndex_ = -1;
    std::mutex mutex_;
    std::condition_variable conditionVariable_;

    DecoderState decoderState_ = DecoderState::STATE_UNKNOWN;

    //播放的起始时间
    long m_StartTimeStamp = -1;
    long currentTimeStamp = -1;
    AVFrame *avFrame_;
    AVPacket *avPacket_;

    float duration_;

    float seekPosition_;
    bool seekSuccess_ = false;

    StateCallback stateCallback_;
    ProgressCallback progressCallback_;
};


