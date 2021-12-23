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
    STATE_UNKNOWN,
    STATE_DECODING,
    STATE_PAUSE,
    STATE_STOP
};

enum DecoderMsg {
    MSG_DECODER_INIT_ERROR,
    MSG_DECODER_READY,
    MSG_DECODER_DONE,
    MSG_REQUEST_RENDER,
    MSG_DECODING_TIME
};

class Decoder{
public:
    virtual ~Decoder() = 0;
    virtual void Start() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual void SeekTo(float position) = 0;
    virtual float GetDuration() = 0;
    using FinishCallback = std::function<void(const int status, const float progress)>;


};