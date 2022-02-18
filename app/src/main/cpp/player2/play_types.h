#pragma once

#include "mutex"
#include "thread"
#include "list"
#include "vector"

extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavfilter/avfilter.h>
}

namespace player{

    /**
     * 解复用后的队列
     */
    typedef struct PacketQueue {
        std::mutex mutex;
        std::condition_variable cond;
        bool abort = false;

        AVPacket **packets = nullptr;
//        std::vector<AVPacket*> packets;
        int read_index = 0;
        int write_index = 0;
        int count = 0;
        int total_bytes = 0;
        unsigned int size = 0;
        uint64_t duration = 0;
        uint64_t max_duration = 0;
        AVPacket flush_packet;

    } PacketQueue;

    typedef struct PacketPool {
        int index = 0;
        int size = 0;
        int count = 0;
        AVPacket **packets = nullptr;
    } PacketPool;

    typedef struct FrameQueue {
        std::mutex mutex;
        std::condition_variable cond;
        AVFrame **frames = nullptr;
        int read_index = 0;
        int write_index = 0;
        int count = 0;
        unsigned int size = 0;
        AVFrame flush_frame;
    } FrameQueue;

    typedef struct FramePool {
        int index =0 ;
        int size = 0;
        int count = 0;
        AVFrame *frames = nullptr;
    } FramePool;

}
