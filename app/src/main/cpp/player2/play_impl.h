#pragma once

#include "play_queue.h"
#include "ff_context.h"
#include "clock.h"

namespace player {

    //播放器状态
    typedef enum {
        UNINIT = -1,
        IDEL = 0,
        PLAYING,
        PAUSED,
        SEEKING,
        SEEK_COMPLETE,
        BUFFER_EMPTY,
        BUFFER_FULL
    } PlayStatus;

    class PlayImpl {
        friend class FFContext;
    public:
        PlayImpl();

        void Init(const std::string &url);

        PlayStatus GetPlayStaus() const{
            return playStatus_;
        }

        AVFrame * GetFromFrameQueue() const {
            return GetFrameQueue(videoFrameQueue_);
        }

        int64_t GetVideoFramePts();

        bool HasAudio() const {
            return ffContext_->HasAudio();
        }

        bool IsHardWare() const {
            return isHardWare_;
        }

    private:
        //读取线程
        void readThread();
        //解码视频
        void DecodeVideo();

        void ReadTest();

    private:
        PacketQueue *audioPacketQueue_;
        FrameQueue *audioFrameQueue_;
        PacketQueue *videoPacketQueue_;
        FrameQueue *videoFrameQueue_;
        PacketPool *packetPool_;
        FramePool *videoFramePool_;
        FramePool *audioFramePool_;

        std::shared_ptr<FFContext> ffContext_;

        //线程处理
        std::thread readThread_;//读取线程
        std::thread videoThread_;
        std::thread audioThread_;

        volatile bool abortRequest;
        volatile bool endOfStream_ = false;

        PlayStatus playStatus_ = PlayStatus ::UNINIT;
        int c = 0;
        bool isHardWare_ = false;

    public:
        AVFrame *videoFrame_;
        Clock *videoClock_;
    };

}

