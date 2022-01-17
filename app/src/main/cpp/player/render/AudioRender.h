#pragma once

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <thread>
#include "queue"

#define MAX_QUEUE_BUFFER_SIZE 3


class AudioFrame{
public:
    AudioFrame(uint8_t * data, int dataSize, bool hardCopy = true) {
        this->dataSize = dataSize;
        this->data = data;
        this->hardCopy = hardCopy;
        if(hardCopy) {
            this->data = static_cast<uint8_t *>(malloc(this->dataSize));
            memcpy(this->data, data, dataSize);
        }
    }

    ~AudioFrame() {
        if(hardCopy && this->data)
            free(this->data);
        this->data = nullptr;
    }

    uint8_t * data = nullptr;
    int dataSize = 0;
    bool hardCopy = true;
};


/**
 * 采用opensl播放音频
 */
class AudioRender {
public:
    AudioRender();
    ~AudioRender();

    void Init();
    void UnInit();
    void RenderFrame(uint8_t *data,int size);

private:
    void StartRender();
    int GetAudioFrameQueueSize();
    void HandleAudoFrameQueue();
    static void AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context);

private:
    //引擎对象
    SLObjectItf engineObject = nullptr;
    SLEngineItf engineEngine = nullptr;

    //混音器
    SLObjectItf outputMixObj = nullptr;
    SLEnvironmentalReverbItf slEnvironmentalReverbItf;

    //播放器和音量控制对象
    SLObjectItf playerObject = nullptr;
    SLPlayItf  playItf = nullptr;
    SLVolumeItf playVolume = nullptr;
    SLAndroidSimpleBufferQueueItf bufferQueueItf = nullptr;

    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::queue<AudioFrame *> audioFrameQueue_;

    volatile bool exit_ = false;
    bool isInitSuccess_ = false;

};


