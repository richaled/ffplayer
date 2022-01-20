#include <log/log.h>
#include "play_impl.h"
#include "android_ff_context.h"
#include <unistd.h>

#define DEFAULT_BUFFER_SIZE 1024*1024*5


namespace player {


    PlayImpl::PlayImpl() {
#ifdef ANDROID
        ffContext_ = std::make_shared<AndroidFFContext>();
#endif
    }

    void PlayImpl::Init(const std::string &url) {
        audioPacketQueue_ = PacketQueueCreate(100);
        audioFrameQueue_ = FrameQueueCreate(100);
        videoPacketQueue_ = PacketQueueCreate(100);
        videoFrameQueue_ = FrameQueueCreate(100);

        packetPool_ = PacketPoolCreate(20);
        videoFramePool_ = FramePoolCreate(4);
        audioFramePool_ = FramePoolCreate(8);

        ffContext_->InitFFmpeg();

        int ret = ffContext_->InitAvContext(url.c_str(), false);
        if(ret != 0){
            LOGE("init avcontext fail");
            return;
        }

        auto readFunc = [&](){
            readThread();
        };
        readThread_ = std::thread(readFunc);

        //开启线程
        if(ffContext_->HasAudio()){
            auto func = [&](){

            };
            audioThread_ = std::thread(func);
        }

        if(ffContext_->HasVideo()){
            auto videoFunc = [&](){
                //decode
                DecodeVideo();
            };
            videoThread_ = std::thread(videoFunc);
        }


    }

    void PlayImpl::readThread() {
        AVPacket *packet = nullptr;
        while (!abortRequest){
            if(audioPacketQueue_->total_bytes + videoPacketQueue_->total_bytes >= DEFAULT_BUFFER_SIZE){
                //改变状态暂停播放，线程暂停读取 todo
                continue;
            }
            if (packet == nullptr) {
                //从pool中获取
                packet = GetPacketFromPool(packetPool_);
            }
            int ret = av_read_frame(ffContext_->GetFormatContext(), packet);
            if(ret == 0){

//                LOGI("read packet %ld,stream index = %d",packet->pts,packet->stream_index);
                //如果是音频
                if(packet->stream_index == ffContext_->GetAudioIndex()){

                } else if(packet->stream_index == ffContext_->GetVideoIndex()){
                    LOGI("add video queue %ld",packet->pts);
                    //添加到队列中
                    PacketPutInQueue(videoPacketQueue_,packet);
                    packet = nullptr;
                }else{
                    UnRefPacketFromPool(packetPool_, packet);
                }
            } else if(ret == AVERROR_INVALIDDATA){
                UnRefPacketFromPool(packetPool_, packet);
            } else if(ret == AVERROR_EOF){
                //通知文件读取完成
                LOGI("read end of file");
                endOfStream_ = true;
                break;
            } else{
                //回调错误 todo

                break;
            }
        }
    }

    void PlayImpl::DecodeVideo() {
        AVFrame* frame = GetFrameFromPool(videoFramePool_);
        int ret = 0;
        while (!abortRequest){
            //111111
            ret = avcodec_receive_frame(ffContext_->videoCodecContext_, frame);
            LOGI("receive frame ret : %s" ,av_err2str(ret));
            if(ret == 0){
                LOGI("receive frame pts : %ld",frame->pts);
                //添加到队列中
//                PutFrameInQueue(videoFrameQueue_, frame);
            }else if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
                AVPacket *packet = GetPacketFromQueue(videoPacketQueue_);
                if(packet == nullptr){
                    LOGI("get queue packet is empty");
                    if(endOfStream_){
                        ret = avcodec_send_packet(ffContext_->videoCodecContext_, packet);
                        if(ret == AVERROR_EOF){
                            break;
                        }else{
                            continue;
                        }
                    }else{
                        usleep(10000);
                        continue;
                    }
                }
                ret = avcodec_send_packet(ffContext_->videoCodecContext_, packet);
                UnRefPacketFromPool(packetPool_, packet);
                if(ret < 0){
                    //回调错误 todo
                    LOGE("send packet error %s",av_err2str(ret));
                    break;
                }
            }else if(ret == AVERROR(EINVAL)){
                //回调错误 todo
                LOGE("receive error %s",av_err2str(ret));
                break;
            }else{
                //回调错误 todo
                LOGE("receive error %s",av_err2str(ret));
                break;
            }
        }
    }

    void PlayImpl::PutFrameInQueue(FrameQueue *queue, AVFrame *frame) {
        std::unique_lock<std::mutex> lock(queue->mutex);
        while (queue->count == queue->size){
            queue->cond.wait(lock);
        }
        queue->frames[queue->write_index] = frame;
        queue->write_index = (queue->write_index + 1) % queue->size;
        queue->count++;
        lock.unlock();
    }


}