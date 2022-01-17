#pragma once

#include "Decoder.h"

class AudioDecoder : public Decoder{
public:
    AudioDecoder();
    ~AudioDecoder();
    void Start() override ;
    void Pause() override ;
    void Stop() override ;
    void SeekTo(float position) override ;
    float GetDuration() override ;

private:

};


