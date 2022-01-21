#pragma once

#include "play_queue.h"
#include "ff_context.h"

namespace player {

    class PlayImpl {
        friend class FFContext;
    public:
        PlayImpl();

        void Init(const std::string &url);

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


        int c = 0;
    };

}

