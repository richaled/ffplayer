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

    int PlayImpl::Init(const test::MediaClip &meidaClip,const test::Options &options) {

        CreateFrameQueue(videoFrameQueue_,4);
        CreateFrameQueue(audioFrameQueue_,6);
        CreatePacketQueue(videoPacketQueue_,40);
        CreatePacketQueue(audioPacketQueue_,40);

        THREAD_ID
        CreatePacketPool(packetPool_,50);
        CreateFramePool(videoFramePool_,8);
        CreateFramePool(audioFramePool_,8);


        videoClock_ = clock_create();
        extClock_ = clock_create();
        isHardWare_ = OptionsGet(options,test::OptionKey::kEnableHardware, false);
        LOGI("hardware enable : %d",isHardWare_);
        ffContext_->InitFFmpeg();
        int ret = ffContext_->InitAvContext(meidaClip.file_name.c_str(), isHardWare_);
        if(ret != 0){
            LOGE("init avcontext fail");
            return ret;
        }
        playStatus_ = PlayStatus::IDEL;
        return ret;
    }

    void PlayImpl::Start() {
        readThread_ = std::thread([this](){
            ReadThread();
        });
        //开启线程
        if(ffContext_->HasAudio()){
            audioThread_ = std::thread([this](){
            });
        }

        if(ffContext_->HasVideo()){
            videoThread_ = std::thread([this](){
                DecodeVideo();
            });
        }
        playStatus_ = PlayStatus::PLAYING;
    }

    void PlayImpl::ReadThread() {
        AVPacket *packet = nullptr;
        while (!abortRequest){

            //向前移动的seek，晴空万里缓存
            if(seekStatus_ == 1){
                LOGI("read seeking ----");
//                ClearFrameQueue();
                seekStatus_ = 2;
                int seek_ret = av_seek_frame(ffContext_->formatContext_, -1, (int64_t) (seekToTime_ * AV_TIME_BASE / 1000),
                                             AVSEEK_FLAG_BACKWARD);
                if (seek_ret < 0) {
                    LOGE("seek faild");
                    continue;
                }
            }

            if(audioPacketQueue_.total_bytes + videoPacketQueue_.total_bytes >= DEFAULT_BUFFER_SIZE){
                //改变状态暂停播放，线程暂停读取 todo
                LOGI("pause read thread");
                //
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
                    //添加到队列中
                    PutPacketInQueue(videoPacketQueue_,packet);
                    packet = nullptr;
//                    std::unique_lock<std::mutex> lock(videoPacketQueue_->mutex);
//                    LOGI("add video packet in queue %ld",packet->pos);
//                    lock.unlock();
//                    packet = nullptr;
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

       /* while (1){
            AVPacket *packet = GetPacketFromQueue(videoPacketQueue_);
            if(packet == nullptr){
                continue;
            }
            LOGI("get packet %ld",packet->pos);
        }*/

        AVFrame* frame = GetFrameFromPool(videoFramePool_);
        int ret = 0;
        while (!abortRequest){
            ret = avcodec_receive_frame(ffContext_->videoCodecContext_, frame);
//            LOGI("receive frame ret : %s" ,av_err2str(ret));
            if(ret == 0){
                LOGI("receive frame pts : %ld",frame->pts);
//                frame->sample_rate =
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
                //seek
                if(packet == &videoPacketQueue_.flush_packet){
                    LOGI("decode flush packet");
                    FrameQueueFlush(videoFrameQueue_,videoFramePool_);
                    avcodec_flush_buffers(ffContext_->videoCodecContext_);
                    continue;
                }

                ret = avcodec_send_packet(ffContext_->videoCodecContext_, packet);
                UnRefPacketFromPool(packetPool_, packet);
                if(ret < 0){
                    //回调错误 todo
                    LOGE("send packet error %s",av_err2str(ret));
                    break;
                }
//                LOGI("send packet success ");
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
        LOGI("video decode end..");
        decodeEnd_ = true;
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

    void PlayImpl::Resume() {
        playStatus_ = PLAYING;
    }

    void PlayImpl::Pause() {
        playStatus_ = PAUSED;
    }

    void PlayImpl::Stop() {
        if(playStatus_ == IDEL){
            return;
        }
        if(abortRequest){
            return;
        }
        //clean queue
        ClearQueues();
        if(readThread_.joinable()){
            readThread_.join();
        }

        //停止线程
        if(HasAudio()){
            if(audioThread_.joinable()){
                audioThread_.join();
            }
        }
        if(HasVideo()){
            if(videoThread_.joinable()){
                videoThread_.join();
            }
        }
        ClearQueues();
        if(ffContext_){
            ffContext_->Close();
        }

    }

    void PlayImpl::ClearQueues() {
        AVPacket *packet;
        // clear context->audio_frame audio_frame_queue  audio_packet_queue
        if (HasAudio()) {
            /*if (context->audio_frame != NULL) {
                if (context->audio_frame != &context->audio_frame_queue->flush_frame) {
                    frame_pool_unref_frame(context->audio_frame_pool, context->audio_frame);
                }
            }*/
            while (1) {
                auto audio_frame = GetFrameQueue(audioFrameQueue_);
                if (audio_frame == nullptr) {
                    break;
                }
                if (audio_frame != &audioFrameQueue_.flush_frame) {
                    UnrefFrameFromPool(audioFramePool_, audio_frame);
                }
            }
            while (1) {
                packet = GetPacketFromQueue(audioPacketQueue_);
                if (packet == nullptr) {
                    break;
                }
                if (packet != &audioPacketQueue_.flush_packet) {
                    UnRefPacketFromPool(packetPool_, packet);
                }
            }
        }
        // clear context->video_frame video_frame_queue video_frame_packet
        if (HasVideo()) {
            if (videoFrame_ != NULL) {
                if (videoFrame_ != &videoFrameQueue_.flush_frame) {
                    UnrefFrameFromPool(videoFramePool_, videoFrame_);
                }
            }
            while (1) {
                videoFrame_ = GetFrameQueue(videoFrameQueue_);
                if (videoFrame_ == nullptr) {
                    break;
                }
                if (videoFrame_ != &videoFrameQueue_.flush_frame) {
                    UnrefFrameFromPool(videoFramePool_,videoFrame_);
                }
            }
            while (1) {
                packet = GetPacketFromQueue(videoPacketQueue_);
                if (packet == nullptr) {
                    break;
                }
                if (packet != &videoPacketQueue_.flush_packet) {
                    UnRefPacketFromPool(packetPool_,packet);
                }
            }
        }
    }

    bool PlayImpl::SeekTo(int64_t seekTime) {
        int64_t previousSeekTime = seekToTime_;
        int64_t totalTime = ffContext_->formatContext_->duration / 1000;
        seekTime = seekTime >= 0 ? seekTime : 0;
        seekTime = seekTime <= totalTime ? seekTime : totalTime;
        seekToTime_ = seekTime;

        //如果是文件尾 todo
        if(endOfStream_){

        }
        bool seekAble = true;
        preciseSeek_ = previousSeekTime > seekTime;
        LOGI("previousSeekTime : %ld,seekTime %ld" ,previousSeekTime,seekTime);
        seekStatus_ = 1;
        //后退
        if(previousSeekTime > seekTime){
            //flush 队列
            FlushPacketQueue();
        }else{
            // 刷出小于seek值已经解码的音频
        }

        //从队列中拿到第一个frame, 并且不是flushframe
        AVFrame *frame = PeekFrameQueue(videoFrameQueue_);
        if(frame && frame !=  &videoFrameQueue_.flush_frame){
            int64_t current_time;
            if(!isHardWare_){
                current_time = av_rescale_q(frame->pts,
                                               ffContext_->formatContext_->streams[ffContext_->videoIndex_]->time_base,
                                               AV_TIME_BASE_Q) /1000;
            } else{
                current_time = frame->pts / 1000;
            }
            LOGI("seekToTime : %ld, current time : %ld",seekToTime_,current_time);
            if(seekToTime_ > current_time){
                //通知发送seek渲染帧消息
                seekAble = true;
                //callback seek
            }
        }
        playStatus_ = SEEKING;
        return seekAble;
    }

    void PlayImpl::FlushPacketQueue() {
        if(HasVideo()){
            PacketQueueFlush(videoPacketQueue_,packetPool_);
        }
        if(HasAudio()){
            PacketQueueFlush(audioPacketQueue_,packetPool_);
        }
    }

    void PlayImpl::ClearFrameQueue() {
        LOGE("enter: %s", __func__);
        while (1) {
            if (videoFrameQueue_.count == 0) {
                break;
            }
            AVFrame* frame = GetFrameQueue(videoFrameQueue_);
            if (!frame) {
                break;
            }
            if (frame == &videoFrameQueue_.flush_frame) {
                break;
            }
            UnrefFrameFromPool(videoFramePool_,frame);
        }
        FrameQueueFlush(videoFrameQueue_, videoFramePool_);
//        frame_queue_flush(context->audio_frame_queue, context->audio_frame_pool);
//        packet_queue_flush(context->audio_packet_queue, context->packet_pool);
//        avcodec_flush_buffers(context->audio_codec_context);
    }

}