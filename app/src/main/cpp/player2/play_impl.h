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
        void readThread();
        //添加读取到的avpacket到队列中
        void PutPacketInQueue(PacketQueue *queue, AVPacket *packet);
        //解码视频
        void DecodeVideo();
        //解码后的视频添加到队列中
        void PutFrameInQueue(FrameQueue*queue,AVFrame *frame);

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


    };

}

