#include <mutex>
#include "ff_context.h"
#include "log/log.h"

namespace player {

    FFContext::FFContext() {

    }

    FFContext::~FFContext() {

    }

    void FFContext::InitFFmpeg() {
        static std::once_flag init_once;
        std::call_once(init_once, []() {
            LOGI("ffmpeg init");
            av_register_all();
            avformat_network_init();
        });
    }

    int FFContext::InitAvContext(const char *url, const bool hardWare) {
        formatContext_ = avformat_alloc_context();
        int ret = 0;
        if ((ret = avformat_open_input(&formatContext_, url, nullptr, nullptr)) != 0) {
            LOGE("can not open url: %s\n", av_err2str(ret));
            return -100;
        }
        if (avformat_find_stream_info(formatContext_, nullptr) < 0) {
            LOGE("can not find stream\n");
            return -100;
        }

        int i = av_find_best_stream(formatContext_, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
        if (i != AVERROR_STREAM_NOT_FOUND) {
            videoIndex_ = i;
            trackFlags |= VIDEO_FLAG;
        }
        i = av_find_best_stream(formatContext_, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
        if (i != AVERROR_STREAM_NOT_FOUND) {
            audioIndex_ = i;
            trackFlags |= AUDIO_FLAG;
        }
        duration_ = formatContext_->duration / AV_TIME_BASE * 1000;
        AVStream *vStream = formatContext_->streams[videoIndex_];
        AVDictionaryEntry *m = nullptr;
        m = av_dict_get(vStream->metadata, "rotate", m, AV_DICT_MATCH_CASE);
        if(m != nullptr){
            frameRotation_ = atoi(m->value);
            LOGI("frame rotate : %d",frameRotation_);
        }

        //如果有音频
        if (HasAudio()) {
            ret = InitAudioCodec();
            if(ret < 0){
                LOGE("audio codec openfail");
                return ret;
            }
        }
        //如果有视频
        if (HasVideo()) {
            ret = InitVideoCodec(hardWare);
            //如果解码失败，硬解切软解
            bool isHardWare = !hardWare;
            if(ret != 0){
                ret = InitVideoCodec(isHardWare);
            }
        }
        return ret;
    }


    int FFContext::InitVideoCodec(const bool hardWare) {
        if(hardWare){
            return InitHardWareCodec();
        }
        //软解
        AVCodecParameters *codecpar = formatContext_->streams[videoIndex_]->codecpar;
        videoCodec_ = avcodec_find_decoder(codecpar->codec_id);
        if(videoCodec_ == nullptr){
            return -1;
        }
        videoCodecContext_ = avcodec_alloc_context3(videoCodec_);
        if(avcodec_parameters_to_context(videoCodecContext_, codecpar) != 0){
            LOGE("InitVideoCodec avcodec_parameters_to_context fail.");
            return -2;
        }
        AVDictionary *options = nullptr;
        av_dict_set(&options, "threads", "auto", 0);
        av_dict_set(&options, "refcounted_frames", "1", 0);
        if (avcodec_open2(videoCodecContext_, videoCodec_, &options) < 0) {
            avcodec_free_context(&videoCodecContext_);
            LOGE("could not open video codec\n");
            return -3;
        }

        AVStream *v_stream = formatContext_->streams[videoIndex_];
        AVDictionaryEntry *m = nullptr;
        m = av_dict_get(v_stream->metadata, "rotate", m, AV_DICT_MATCH_CASE);
        if (m != nullptr) {
            frameRotation_ = atoi(m->value);
        }
        return 0;
    }

    int FFContext::InitAudioCodec() {
        audioCodec_ = avcodec_find_decoder(formatContext_->streams[audioIndex_]->codecpar->codec_id);
        if(audioCodec_ == nullptr){
            return -1;
        }
        audioCodecContext_ = avcodec_alloc_context3(audioCodec_);
        if(avcodec_parameters_to_context(audioCodecContext_, formatContext_->streams[audioIndex_]->codecpar) != 0){
            LOGE("InitAudioCodec avcodec_parameters_to_context fail.");
            return -2;
        }
        if (avcodec_open2(audioCodecContext_, audioCodec_, nullptr) < 0) {
            avcodec_free_context(&audioCodecContext_);
            LOGE("could not open audio codec\n");
            return -3;
        }
        //设置声音filter todo


        return 0;
    }

    void FFContext::Close() {
        if(HasAudio()){
            avcodec_close(audioCodecContext_);
            avcodec_free_context(&audioCodecContext_);
        }

        if(HasVideo()){
            avcodec_close(videoCodecContext_);
            avcodec_free_context(&videoCodecContext_);
        }
        avformat_close_input(&formatContext_);
        avformat_free_context(formatContext_);
        formatContext_ = nullptr;

        width_ = 0;
        height_ = 0;
        frameRotation_ = 0;

    }

    /*static int av_format_interrupt_cb(void *data) {
        AVPlayContext* context = data;
        if (context->timeout_start == 0) {
            context->timeout_start = clock_get_current_time();
            return 0;
        } else {
            uint64_t time_use = clock_get_current_time() - context->timeout_start;
            if (time_use > context->read_timeout * 1000000) {
                context->on_error(context, -2);
                return 1;
            } else {
                return 0;
            }
        }
    }*/
}