#pragma once

#include "mutex"
#include "thread"

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

        AVPacket **packets;
        int read_index;
        int write_index;
        int count;
        int total_bytes;
        unsigned int size;
        uint64_t duration;
        uint64_t max_duration;
        AVPacket flush_packet;

    } PacketQueue;

    typedef struct PacketPool {
        int index = 0;
        int size;
        int count = 0;
        AVPacket **packets;
    } PacketPool;

    typedef struct FrameQueue {
        std::mutex mutex;
        std::condition_variable cond;
        AVFrame **frames;
        int read_index = 0;
        int write_index = 0;
        int count = 0;
        unsigned int size;
        AVFrame flush_frame;
    } FrameQueue;

    typedef struct FramePool {
        int index;
        int size;
        int count;
        AVFrame *frames;
    } FramePool;

}
