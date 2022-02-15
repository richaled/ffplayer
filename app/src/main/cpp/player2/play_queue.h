#pragma once
#include "play_types.h"

namespace player{
    /**
    * 创建队列
    * @param size
    * @return
    */
    PacketQueue* PacketQueueCreate(unsigned int size);
    void PacketQueueFree(PacketQueue* queue);
    int PacketPutInQueue(PacketQueue *queue, AVPacket *packet);
    AVPacket *GetPacketFromQueue(PacketQueue *queue);

    PacketPool* PacketPoolCreate(int size);
    void PacketPoolFree(PacketPool *pool);
    AVPacket *GetPacketFromPool(PacketPool*pool);
    void UnRefPacketFromPool(PacketPool*pool,AVPacket *packet);


    /**
     * 创建frame的池
     * @param size
     * @return
     */
    FramePool* FramePoolCreate(int size);
    AVFrame* GetFrameFromPool(FramePool *pool);
    void UnrefFrameFromPool(FramePool *pool, AVFrame *frame);


    /**
     * 创建frame队列
     * @param size
     * @return
     */
    FrameQueue* FrameQueueCreate(unsigned int size);
    void FrameQueueFree(FrameQueue *queue);
    AVFrame* GetFrameQueue(FrameQueue *queue);
    int PutFrameQueue(FrameQueue *queue, AVFrame *frame);




    }


