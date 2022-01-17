#include "DecoderBase.h"
#include <log/log.h>


DecoderBase::~DecoderBase() {

}

void DecoderBase::Start() {
    StartDecodingThread();
}

void DecoderBase::Stop() {
    std::unique_lock<std::mutex> lock(mutex_);
    decoderState_ = DecoderState::STATE_STOP;
    conditionVariable_.notify_all();
}

void DecoderBase::Pause() {
    std::unique_lock<std::mutex> lock(mutex_);
    decoderState_ = DecoderState::STATE_PAUSE;
}

void DecoderBase::SeekTo(float position) {

}

float DecoderBase::GetDuration() {
    return 0;
}

void DecoderBase::StartDecodingThread() {
    auto loop_func = [&]() {
        if (InitFFDecoder() != 0){
            //callback todo

            return;
        }
        OnDecoderReady();
        //音视频解码循环
        DecodingLoop();
        UnInitDecoder();
        OnDecoderDone();

    };
    thread_ = std::thread(loop_func);
}

int DecoderBase::InitFFDecoder() {
    avFormatContext_ = avformat_alloc_context();
    int result = 0;
    if((result = avformat_open_input(&avFormatContext_,url_.c_str(), nullptr, nullptr)) != 0){
        LOGE("input open fail : %d" ,result);
        return static_cast<int >(DecoderMsg::MSG_DECODER_INIT_ERROR);
    }
    if(avformat_find_stream_info(avFormatContext_, nullptr) < 0){
        LOGE("iformat find stream info");
        return static_cast<int >(DecoderMsg::MSG_DECODER_INIT_ERROR);
    }
    //4.获取音视频流索引
    for(int i=0; i < avFormatContext_->nb_streams; i++) {
        if(avFormatContext_->streams[i]->codecpar->codec_type == mediaType_) {
            streamIndex_ = i;
            break;
        }
    }
    if(streamIndex_ == -1) {
        LOGE("DecoderBase::InitFFDecoder Fail to find stream index.");
        return static_cast<int >(DecoderMsg::MSG_DECODER_INIT_ERROR);
    }
    //获取解码器参数
    AVCodecParameters *codecParameters = avFormatContext_->streams[streamIndex_]->codecpar;
    avCodec_ = avcodec_find_decoder(codecParameters->codec_id);
//    avCodec_ = avcodec_find_decoder_by_name("h264");
    if(avCodec_ == nullptr){
        LOGE("DecoderBase::InitFFDecoder avcodec_find_decoder fail.");
        return static_cast<int >(DecoderMsg::MSG_DECODER_INIT_ERROR);
    }

    avCodecContext_ = avcodec_alloc_context3(avCodec_);
    if(avcodec_parameters_to_context(avCodecContext_, codecParameters) != 0){
        LOGE("DecoderBase::InitFFDecoder avcodec_parameters_to_context fail.");
        return static_cast<int >(DecoderMsg::MSG_DECODER_INIT_ERROR);
    }
    result = avcodec_open2(avCodecContext_,avCodec_, nullptr);
    if(result < 0){
        LOGE("DecoderBase::InitFFDecoder avcodec_open2 fail. result=%d", result);
        return static_cast<int >(DecoderMsg::MSG_DECODER_INIT_ERROR);
    }
    LOGI("codec open success");
    //获取视频的时长
    duration_ = avFormatContext_->duration / AV_TIME_BASE * 1000;

    return result;
}

void DecoderBase::UnInitDecoder() {
    if(avFormatContext_ != nullptr){
        avformat_close_input(&avFormatContext_);
        avformat_free_context(avFormatContext_);
        avFormatContext_ = nullptr;
    }

    if(avCodecContext_ != nullptr){
        avcodec_close(avCodecContext_);
        avcodec_free_context(&avCodecContext_);
        avCodecContext_ = nullptr;
    }
}

void DecoderBase::DecodingLoop() {
    //循环解码
    {
        std::unique_lock<std::mutex> lock(mutex_);
        decoderState_ = DecoderState::STATE_DECODING;
        lock.unlock();
    }

    for (;;) {
        while (decoderState_ == DecoderState::STATE_PAUSE){
            //等待
            std::unique_lock<std::mutex> lock(mutex_);
            conditionVariable_.wait_for(lock, std::chrono::milliseconds(10));
            //获取
        }
        if(decoderState_ == DecoderState::STATE_STOP){
            break;
        }
        if(m_StartTimeStamp == -1){
            m_StartTimeStamp = GetSysCurrentTime();
        }
        if(DecodeOnePacket() != 0){
            std::unique_lock<std::mutex> lock(mutex_);
            decoderState_ = DecoderState::STATE_PAUSE;
        }
    }
}

int DecoderBase::DecodeOnePacket() {
    AVPacket *avPacket = av_packet_alloc();
    avFrame_ = av_frame_alloc();
    int result = av_read_frame(avFormatContext_,avPacket);
    while (result == 0){
        if(avPacket->stream_index == streamIndex_){
            if(avcodec_send_packet(avCodecContext_,avPacket) == AVERROR_EOF){
                result = -1;
                goto FINISH;
            }
            long frameCount = 0;
            while (avcodec_receive_frame(avCodecContext_,avFrame_) == 0){
                LOGI("receive frame : %ld",avFrame_->pts);
                //更新时间戳
                UpdateTimeStamp();
//                同步
                AVSync();
                //渲染
                onFrameAvailable(avFrame_);
                frameCount ++;
            }
            if(frameCount > 0){
                result = 0;
                goto FINISH;
            }
        }
        av_packet_unref(avPacket);
        result = av_read_frame(avFormatContext_,avPacket);
    }
    FINISH:
    av_packet_unref(avPacket);
    return result;
}

void DecoderBase::UpdateTimeStamp() {
    std::unique_lock<std::mutex> lock(mutex_);
    if(avFrame_->pkt_dts != AV_NOPTS_VALUE){
        currentTimeStamp = avFrame_->pkt_dts;
    }else if(avFrame_->pts != AV_NOPTS_VALUE){
        currentTimeStamp = avFrame_->pts;
    }else {
        currentTimeStamp = 0;
    }
    currentTimeStamp = currentTimeStamp * av_q2d(avFormatContext_->streams[streamIndex_]->time_base) * 1000;
}

long DecoderBase::AVSync() {
    long curSysTime = GetSysCurrentTime();
    long elapsedTime = curSysTime - m_StartTimeStamp;
    //回调

    long delay = 0;
    if(curSysTime > elapsedTime){
        auto sleepTime = static_cast<unsigned int>(currentTimeStamp - elapsedTime);//ms}
        sleepTime = sleepTime > DELAY_THRESHOLD ? DELAY_THRESHOLD : sleepTime;
        av_usleep(sleepTime * 1000);
    }
    delay = elapsedTime - currentTimeStamp;
    return delay;
}
