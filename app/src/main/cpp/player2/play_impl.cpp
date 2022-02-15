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

    int PlayImpl::Init(const std::string &url,const test::Options &options) {
        audioPacketQueue_ = PacketQueueCreate(40);
        audioFrameQueue_ = FrameQueueCreate(40);
        videoPacketQueue_ = PacketQueueCreate(40);
        videoFrameQueue_ = FrameQueueCreate(60);

        packetPool_ = PacketPoolCreate(20);
        videoFramePool_ = FramePoolCreate(8);
        audioFramePool_ = FramePoolCreate(8);

        videoClock_ = clock_create();
        isHardWare_ = OptionsGet(options,test::OptionKey::kEnableHardware, false);
        LOGI("hardware enable : %d",isHardWare_);
        ffContext_->InitFFmpeg();
        int ret = ffContext_->InitAvContext(url.c_str(), isHardWare_);
        if(ret != 0){
            LOGE("init avcontext fail");
            return ret;
        }
        playStatus_ = PlayStatus::IDEL;
        return ret;
    }

    void PlayImpl::Start() {
        LOGI("play thread start");
        auto readFunc = [&](){
            ReadThread();
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
//                ReadTest();
            };
            videoThread_ = std::thread(videoFunc);
        }
        playStatus_ = PlayStatus::PLAYING;
    }

    void PlayImpl::ReadThread() {
        AVPacket *packet = nullptr;
        while (!abortRequest){
            if(audioPacketQueue_->total_bytes + videoPacketQueue_->total_bytes >= DEFAULT_BUFFER_SIZE){
                //改变状态暂停播放，线程暂停读取 todo
                LOGI("pause read thread");
                continue;
            }
            if (packet == nullptr) {
                //从pool中获取
                packet = GetPacketFromPool(packetPool_);
            }
            int ret = av_read_frame(ffContext_->GetFormatContext(), packet);
            if(ret == 0){

//                LOGI("read packet %ld,stream index = %d",packet->duration,packet->stream_index);
                //如果是音频
                if(packet->stream_index == ffContext_->GetAudioIndex()){

                } else if(packet->stream_index == ffContext_->GetVideoIndex()){
                    LOGI("add video queue %ld",packet->pos);
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
//            LOGI("receive frame ret : %s" ,av_err2str(ret));
            if(ret == 0){
                LOGI("receive frame pts : %ld",frame->pts);
                //添加到队列中
                PutFrameQueue(videoFrameQueue_, frame);
                usleep(2000);
                frame = GetFrameFromPool(videoFramePool_);
            }else if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
                AVPacket *packet = GetPacketFromQueue(videoPacketQueue_);
                if(packet == nullptr){
//                    LOGI("get queue packet is empty");
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
                LOGI("send packet success ");
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

    void PlayImpl::ReadTest() {
        while (true){
            AVPacket *packet = GetPacketFromQueue(videoPacketQueue_);
            if(packet == nullptr){
                LOGE("read packet empty");
                usleep(10000);
            } else{
                LOGI("read packet :%ld,count %d",packet->duration,c++);
                if(c > 30){
                    LOGE("discard packet");
                    break;
                }
            }
        }
    }

    int64_t PlayImpl::GetVideoFramePts() {
        int64_t video_frame_pts;
        if(!isHardWare_){
            video_frame_pts = av_rescale_q(videoFrame_->pts,
                                           ffContext_->formatContext_->streams[ffContext_->videoIndex_]->time_base,
                                           AV_TIME_BASE_Q);
        } else{
            video_frame_pts = videoFrame_->pts;
        }
        return video_frame_pts;
    }


}