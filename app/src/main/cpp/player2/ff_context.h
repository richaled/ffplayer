#pragma once

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavfilter/avfilter.h>
}

#if __ANDROID_API__ >= NDK_MEDIACODEC_VERSION
    #include <media/NdkMediaCodec.h>
#endif

#define AUDIO_FLAG 0x1
#define VIDEO_FLAG 0x2

namespace player {

    typedef struct MediaCodecContext {
#if __ANDROID_API__ >= NDK_MEDIACODEC_VERSION
        AMediaCodec *codec;
        AMediaFormat *format;
#endif
        size_t nal_size;
        int width, height;
        enum AVPixelFormat pix_format;
        enum AVCodecID codec_id;
    } MediaCodecContext;

    class FFContext {
    public:
        FFContext();
        virtual ~FFContext();

        /**
         * 初始化ffmpeg
         */
        void InitFFmpeg();

        int InitAvContext(const char*url, const bool hardWare = false);

        bool HasVideo() const {
            return trackFlags & VIDEO_FLAG;
        }

        bool HasAudio() const {
            return trackFlags & AUDIO_FLAG;
        }

        AVFormatContext *GetFormatContext() const {
            return formatContext_;
        }

        int GetVideoIndex() const{
            return videoIndex_;
        }
        int GetAudioIndex() const {
            return audioIndex_;
        }

    protected:
        //各个平台去实现
        virtual int InitHardWareCodec() = 0;

    private:
        int InitVideoCodec(const bool hardWare);

        int InitAudioCodec();

    public:
        AVFormatContext *formatContext_;
        int videoIndex_ = -1;
        int audioIndex_ = -1;
        float duration_;
        uint8_t trackFlags;

        //音频
        AVCodec *audioCodec_;
        AVCodecContext *audioCodecContext_;
//        AudioFilterContext *audioFilterContext;

        //视频
        AVCodec *videoCodec_;
        AVCodecContext *videoCodecContext_;


        int width_;
        int height_;
        int frameRotation_ = 0;


    };
}


