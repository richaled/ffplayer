#pragma once
#include "play_types.h"

namespace player{
    /**
    * 创建队列
    * @param size
    * @return
    */
    void CreatePacketQueue(PacketQueue &queue, unsigned int size);
    int PutPacketInQueue(PacketQueue &queue, AVPacket *packet);
    AVPacket *GetPacketFromQueue(PacketQueue &queue);
    void PacketQueueFree(PacketQueue& queue);


//    PacketPool* PacketPoolCreate(int size);
    void CreatePacketPool(PacketPool &packetPool,int size);
    void PacketPoolFree(PacketPool &pool);
    AVPacket *GetPacketFromPool(PacketPool&pool);
    void UnRefPacketFromPool(PacketPool&pool,AVPacket *packet);
    void PacketQueueFlush(PacketQueue &queue,PacketPool &pool);
    void PacketQueueFlush(PacketQueue &queue,PacketPool &pool);


    /**
     * 创建frame的池
     * @param size
     * @return
     */
//    FramePool* FramePoolCreate(int size);
    void CreateFramePool(FramePool &framePool, int size);
    AVFrame* GetFrameFromPool(FramePool &pool);
    void UnrefFrameFromPool(FramePool &pool, AVFrame *frame);


    /**
     * 创建frame队列
     * @param size
     * @return
     */
    void CreateFrameQueue(FrameQueue &queue, unsigned int size);
//    FrameQueue* FrameQueueCreate(unsigned int size);
    void FrameQueueFree(FrameQueue &queue);
    AVFrame* GetFrameQueue(FrameQueue &queue);
    int PutFrameQueue(FrameQueue &queue, AVFrame *frame);
    AVFrame* PeekFrameQueue(FrameQueue& queue);
    void FrameQueueFlush(FrameQueue &queue, FramePool &pool);
    AVFrame *PeekLastFrame(FrameQueue &queue);


}


