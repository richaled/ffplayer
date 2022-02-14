#include <log/log.h>
#include "play_queue.h"

namespace player {

    PacketQueue* PacketQueueCreate(unsigned int size){
        PacketQueue *queue = (PacketQueue*) malloc(sizeof(PacketQueue));
        queue->read_index = 0;
        queue->write_index = 0;
        queue->count = 0;
        queue->size = size;
        queue->duration = 0;
        queue->total_bytes = 0;
        queue->max_duration = 8000000;
        queue->flush_packet.duration = 0;
        queue->flush_packet.size = 0;
        queue->packets = (AVPacket **) malloc(sizeof(AVPacket *) * size);
        return queue;
    }

    void PacketQueueFree(PacketQueue* queue){
        free(queue->packets);
        free(queue);
    }

    int PacketPutInQueue(PacketQueue *queue, AVPacket *packet) {
        std::unique_lock<std::mutex> lock(queue->mutex);
        /*if(queue->max_duration > 0 && queue->duration + packet->duration > queue->max_duration){
            //等待
            queue->cond.wait(lock);
            LOGI("queue wait");
        }*/
        LOGI("write count : %d" ,queue->count);
        while (queue->count >= 1){
            LOGI("queue write wait");
            queue->cond.wait(lock);
        }
        queue->duration += packet->duration;
        queue->packets[queue->write_index] = packet;
        LOGE("queue write index %d,%p",queue->write_index,packet);
        queue->write_index = (queue->write_index + 1) % queue->size;
        queue->count++;
        queue->total_bytes += packet->size;
        lock.unlock();
        return 0;
    }

    AVPacket *GetPacketFromQueue(PacketQueue *queue){
        std::unique_lock<std::mutex> lock(queue->mutex);
        if(queue->count == 0){
            LOGI("queue get empty");
            lock.unlock();
            return nullptr;
        }
        AVPacket *packet = queue->packets[queue->read_index];
        LOGI("queue read index %d,%p",queue->read_index,packet);
        queue->read_index = (queue->read_index + 1) % queue->size;
        queue->count--;
        queue->duration -= packet->duration;
        queue->total_bytes -= packet->size;
        queue->cond.notify_all();
        lock.unlock();
        return packet;
    }

    PacketPool* PacketPoolCreate(int size){
        PacketPool *pool = (PacketPool*)malloc(sizeof(PacketPool));
        pool->size = size;
        pool->index = 0;
        pool->count = 0;
        pool->packets = (AVPacket **)av_malloc(sizeof(AVPacket *) * size);
        for (int i = 0; i < pool->size; i++){
            pool->packets[i] = av_packet_alloc();
        }
        return pool;
    }

    static void PacketPoolDoubleDize(PacketPool * pool) {
        // step1  malloc new memery space to store pointers |________________|
        AVPacket** temp_packets = (AVPacket **) av_malloc(sizeof(AVPacket *) * pool->size * 2);
        // step2  copy old pointers to new space            |XXXXXXXX________|
        memcpy(temp_packets, pool->packets, sizeof(AVPacket *) * pool->size);
        // step3 fill rest space with av_packet_alloc       |XXXXXXXXOOOOOOOO|
        for (int i = pool->size; i < pool->size * 2; i++) {
            temp_packets[i] = av_packet_alloc();
        }
        // step4 free old pointers space
        free(pool->packets);
        pool->packets = temp_packets;
        // step5 当前指针位置移动到后半部分
        pool->index = pool->size;
        pool->size *= 2;
//        LOGI("packet pool double size. new size ==> %d", pool->size);
    }

    AVPacket* GetPacketFromPool(PacketPool *pool) {
        if (pool->count > pool->size * 0.75) {
            PacketPoolDoubleDize(pool);
        }
        AVPacket* p = pool->packets[pool->index];
        pool->index = (pool->index + 1) % pool->size;
        pool->count++;
        return p;
    }

    void UnRefPacketFromPool(PacketPool*pool,AVPacket *packet){
        av_packet_unref(packet);
        pool->count--;
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

    FramePool* FramePoolCreate(int size){
        FramePool *pool = (FramePool*)malloc(sizeof(FramePool));
        pool->size = size;
        pool->count = 0;
        pool->index = 0;
        pool->frames = (AVFrame *)av_mallocz(sizeof(AVFrame) * size);
        for (int i = 0; i < size; ++i) {
            FrameCreateDefaults(&pool->frames[i]);
        }
        return pool;
    }

    AVFrame* GetFrameFromPool(FramePool *pool){
        AVFrame* p = &pool->frames[pool->index];
        pool->index = (pool->index + 1) % pool->size;
        pool->count++;
        return p;
    }

    void UnrefFrameFromPool(FramePool *pool, AVFrame *frame){
        av_frame_unref(frame);
        pool->count--;
    }

    FrameQueue* FrameQueueCreate(unsigned int size){
        FrameQueue* queue = (FrameQueue *)malloc(sizeof(FrameQueue));
        queue->read_index = 0;
        queue->write_index = 0;
        queue->count = 0;
        queue->size = size;
        queue->frames = (AVFrame **)malloc(sizeof(AVFrame *) * size);
        return queue;
    }

    void FrameQueueFree(FrameQueue *queue){
        free(queue->frames);
        free(queue);
    }

    AVFrame* GetFrameQueue(FrameQueue *queue){
        std::unique_lock<std::mutex> lock(queue->mutex);
        if (queue->count == 0) {
            lock.unlock();
            return NULL;
        }
        AVFrame* frame = queue->frames[queue->read_index];
        queue->read_index = (queue->read_index + 1) % queue->size;
        queue->count--;
        queue->cond.notify_all();
        lock.unlock();
        return frame;
    }

}