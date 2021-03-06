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

        bool SeekTo(int64_t seekTime);

        PlayStatus GetPlayStaus() const{
            return playStatus_;
        }

        AVFrame * GetFromFrameQueue(){
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

        int GetVideoPacketsSize(){
            return videoPacketQueue_.size;
        }

        int GetVideoFrameSize(){
            return videoFrameQueue_.count;
        }

    private:
        //读取线程
        void ReadThread();
        //解码视频
        void DecodeVideo();

        void ClearQueues();

        void FlushPacketQueue();

        void ClearFrameQueue();
    private:
        PacketQueue audioPacketQueue_;
        FrameQueue audioFrameQueue_;
//        PacketQueue *videoPacketQueue_;
        FrameQueue videoFrameQueue_;
        PacketPool packetPool_;
        FramePool audioFramePool_;

        PacketQueue videoPacketQueue_;


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

        int64_t seekToTime_ = 0;
        bool preciseSeek_ = false; //精准seek
        volatile uint8_t seekStatus_ = 0;

    public:
        PlayStatus playStatus_ = PlayStatus ::UNINIT;
        FramePool videoFramePool_;
        AVFrame *videoFrame_ = nullptr;
        Clock *videoClock_;
        Clock *extClock_;
        bool decodeEnd_= false;
    };

}

