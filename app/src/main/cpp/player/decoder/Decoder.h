#pragma once
#include <functional>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
#include <libavcodec/jni.h>
}

enum DecoderState {
    STATE_UNKNOWN = 0,
    STATE_DECODING,
    STATE_PAUSE,
    STATE_STOP
};

enum DecoderMsg {
    MSG_SUCCESS = 0,
    MSG_DECODER_INIT_ERROR = 1,
    MSG_DECODER_READY,
    MSG_DECODER_DONE,
    MSG_REQUEST_RENDER,
    MSG_DECODING_TIME
};

using StateCallback = std::function<void(const int status)>;
using ProgressCallback = std::function<void(const float progress, const float total)>;
class Decoder{
public:
//    Decoder(){};
//    virtual ~Decoder(){};
//    virtual ~Decoder(){};
    virtual void Start() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual void SeekTo(float position) = 0;
    virtual float GetDuration() = 0;

};