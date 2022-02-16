#pragma once

#include "play_queue.h"
#include "ff_context.h"
#include "clock.h"
#include "media_base.h"

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

        int Init(const test::MediaClip &meidaClip, const test::Options &options);

        void Start();

        void Stop();

        void Resume();

        void Pause();

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

        bool HasVideo() const {
            return ffContext_->HasVideo();
        }

        bool IsHardWare() const {
            return isHardWare_;
        }
        int GetFrameRotate() const {
            return ffContext_->frameRotation_;
        }

    private:
        //读取线程
        void ReadThread();
        //解码视频
        void DecodeVideo();

        void ClearQueues();
    private:
        PacketQueue *audioPacketQueue_;
        FrameQueue *audioFrameQueue_;
        PacketQueue *videoPacketQueue_;
        FrameQueue *videoFrameQueue_;
        PacketPool *packetPool_;
        FramePool *audioFramePool_;

        std::shared_ptr<FFContext> ffContext_;

        //线程处理
        std::thread readThread_;//读取线程
        std::thread videoThread_;
        std::thread audioThread_;
        std::mutex eofMutex_;
        std::condition_variable eofCondition_;

        volatile bool abortRequest = false;
        volatile bool endOfStream_ = false;

        int c = 0;
        bool isHardWare_ = false;
    public:
        PlayStatus playStatus_ = PlayStatus ::UNINIT;
        FramePool *videoFramePool_;
        AVFrame *videoFrame_ = nullptr;
        Clock *videoClock_;
    };

}

