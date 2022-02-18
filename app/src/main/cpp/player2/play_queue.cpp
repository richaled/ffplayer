#include <log/log.h>
#include "play_queue.h"

namespace player {

    void CreatePacketQueue(PacketQueue &queue, unsigned int size){
        queue.size = size;
        queue.max_duration = 8000000;
//        queue.packets.resize(size);
        queue.packets = (AVPacket **) malloc(sizeof(AVPacket *) * size);
//        for (int i = 0; i < size; ++i) {
//            queue.packets[i] = av_packet_alloc();
//        }
    }

    void PacketQueueFree(PacketQueue& queue){
        free(queue.packets);
//        queue.packets.clear();
    }

    int PutPacketInQueue(PacketQueue &queue, AVPacket *packet){
        METHOD
        std::unique_lock<std::mutex> lock(queue.mutex);
        if(queue.max_duration > 0 && queue.duration + packet->duration > queue.max_duration){
            //等待
            queue.cond.wait(lock);
        }
        /*if (queue.count >= 1){
            queue.cond.wait(lock);
        }*/
        queue.duration += packet->duration;
        queue.packets[queue.write_index] = packet;
        LOGE("queue write index %d,%p",queue.write_index,packet);
        queue.write_index = (queue.write_index + 1) % queue.size;
        queue.count++;
        queue.total_bytes += packet->size;
        lock.unlock();
        return 0;
    }

    AVPacket *GetPacketFromQueue(PacketQueue &queue){
//        THREAD_ID
        std::unique_lock<std::mutex> lock(queue.mutex);
        if(queue.count == 0){
            LOGI("packet queue get empty");
            lock.unlock();
            return nullptr;
        }
        LOGE("queue read index %d",queue.read_index);
        AVPacket *packet = queue.packets[queue.read_index];
        queue.read_index = (queue.read_index + 1) % queue.size;
        queue.count--;
        queue.duration -= packet->duration;
        queue.total_bytes -= packet->size;
        queue.cond.notify_all();
        lock.unlock();
        return packet;
    }

    void PacketQueueFlush(PacketQueue &queue,PacketPool &pool){
        METHOD
        std::unique_lock<std::mutex> lock(queue.mutex);
        while (queue.count > 0) {
            AVPacket *packet = queue.packets[queue.read_index];
            if (packet != &queue.flush_packet) {
                UnRefPacketFromPool(pool, packet);
            }
            queue.read_index = (queue.read_index + 1) % queue.size;
            queue.count--;
        }
        queue.read_index = 0;
        queue.duration = 0;
        queue.total_bytes = 0;
        queue.packets[0] = &queue.flush_packet;
        queue.write_index = 1;
        queue.count = 1;
        queue.cond.notify_all();
        lock.unlock();
    }

    void CreatePacketPool(PacketPool &pool,int size){
        pool.size = size;
        pool.packets = (AVPacket **)av_malloc(sizeof(AVPacket *) * size);
        for (int i = 0; i < pool.size; i++){
            pool.packets[i] = av_packet_alloc();
        }
    }

    static void PacketPoolDoubleDize(PacketPool& pool) {
        // step1  malloc new memery space to store pointers |________________|
        AVPacket** temp_packets = (AVPacket **) av_malloc(sizeof(AVPacket *) * pool.size * 2);
        // step2  copy old pointers to new space            |XXXXXXXX________|
        memcpy(temp_packets, pool.packets, sizeof(AVPacket *) * pool.size);
        // step3 fill rest space with av_packet_alloc       |XXXXXXXXOOOOOOOO|
        for (int i = pool.size; i < pool.size * 2; i++) {
            temp_packets[i] = av_packet_alloc();
        }
        // step4 free old pointers space
        free(pool.packets);
        pool.packets = temp_packets;
        // step5 当前指针位置移动到后半部分
        pool.index = pool.size;
        pool.size *= 2;
    }

    AVPacket* GetPacketFromPool(PacketPool &pool) {
        if (pool.count > pool.size * 0.75) {
            PacketPoolDoubleDize(pool);
        }
        AVPacket* p = pool.packets[pool.index];
        pool.index = (pool.index + 1) % pool.size;
        pool.count++;
        return p;
    }

    void UnRefPacketFromPool(PacketPool&pool,AVPacket *packet){
        av_packet_unref(packet);
        pool.count--;
    }

    static void FrameCreateDefaults(AVFrame *frame) {
        // from ffmpeg libavutil frame.c
        if (frame->extended_data != frame->data) {
            av_freep(&frame->extended_data);
        }
        memset(frame, 0, sizeof(*frame));
        frame->pts                   =
        frame->pkt_dts               = AV_NOPTS_VALUE;
        frame->best_effort_timestamp = AV_NOPTS_VALUE;
        frame->pkt_duration        = 0;
        frame->pkt_pos             = -1;
        frame->pkt_size            = -1;
        frame->key_frame           = 1;
        frame->sample_aspect_ratio = (AVRational){ 0, 1 };
        frame->format              = -1; /* unknown */
        frame->extended_data       = frame->data;
        frame->color_primaries     = AVCOL_PRI_UNSPECIFIED;
        frame->color_trc           = AVCOL_TRC_UNSPECIFIED;
        frame->colorspace          = AVCOL_SPC_UNSPECIFIED;
        frame->color_range         = AVCOL_RANGE_UNSPECIFIED;
        frame->chroma_location     = AVCHROMA_LOC_UNSPECIFIED;
        frame->flags               = 0;
        frame->width               = 0;
        frame->height              = 0;
    }

    void CreateFramePool(FramePool &pool, int size){
        pool.size = size;
        pool.frames = (AVFrame *)av_mallocz(sizeof(AVFrame) * size);
        for (int i = 0; i < size; ++i) {
            FrameCreateDefaults(&pool.frames[i]);
        }
    }

    AVFrame* GetFrameFromPool(FramePool &pool){
        AVFrame* p = &pool.frames[pool.index];
        pool.index = (pool.index + 1) % pool.size;
        pool.count++;
        return p;
    }

    void UnrefFrameFromPool(FramePool &pool, AVFrame *frame){
        if(frame == nullptr){
            return;
        }
//        av_frame_free(&frame);
        av_frame_unref(frame);
        pool.count--;
    }

    void CreateFrameQueue(FrameQueue &queue, unsigned int size){
        queue.size = size;
        queue.frames = (AVFrame **)malloc(sizeof(AVFrame *) * size);
    }

    void FrameQueueFree(FrameQueue &queue){
        free(queue.frames);
    }

    AVFrame* GetFrameQueue(FrameQueue &queue){
        std::unique_lock<std::mutex> lock(queue.mutex);
        if (queue.count == 0) {
//            queue.cond.notify_all();
            lock.unlock();
            return nullptr;
        }
        AVFrame* frame = queue.frames[queue.read_index];
        queue.read_index = (queue.read_index + 1) % queue.size;
        queue.count--;
        queue.cond.notify_all();
        lock.unlock();
        return frame;
    }

    int PutFrameQueue(FrameQueue &queue, AVFrame *frame){
        std::unique_lock<std::mutex> lock(queue.mutex);
        while(queue.count == queue.size){
//            LOGE("queue count same queue size ,%d",queue.count);
            queue.cond.wait(lock);
        }
        queue.frames[queue.write_index] = frame;
        queue.write_index = (queue.write_index + 1) % queue.size;
        queue.count++;
        lock.unlock();
        return 0;
    }

    AVFrame* PeekFrameQueue(FrameQueue& queue) {
        std::unique_lock<std::mutex> lock(queue.mutex);
        if (queue.count == 0) {
            lock.unlock();
            return nullptr;
        }
        AVFrame* frame = queue.frames[queue.read_index];
        lock.unlock();
        return frame;
    }

    void FrameQueueFlush(FrameQueue &queue, FramePool &pool){
        std::unique_lock<std::mutex> lock(queue.mutex);
        while (queue.count > 0) {
            AVFrame* frame = queue.frames[queue.read_index];
            if (frame != &queue.flush_frame) {
                UnrefFrameFromPool(pool, frame);
            }
            queue.read_index = (queue.read_index + 1) % queue.size;
            queue.count--;
        }
        queue.read_index = 0;
        queue.frames[0] = &queue.flush_frame;
        queue.write_index = 1;
        queue.count = 1;
        queue.cond.notify_all();
        lock.unlock();
    }


}