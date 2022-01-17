#include <log/log.h>
#include "AudioRender.h"

AudioRender::AudioRender() {

}

AudioRender::~AudioRender() {

}

void AudioRender::Init() {
    //创建引擎
    SLresult result = slCreateEngine(&engineObject,0, nullptr,0, nullptr, nullptr);
    if(result != SL_RESULT_SUCCESS){
        LOGE("OpenSLRender::Init CreateEngine fail. result=%d", result);
        return;
    }
    result = (*engineObject)->Realize(engineObject,SL_BOOLEAN_FALSE);//实现engine
    if(result != SL_RESULT_SUCCESS){
        LOGE("OpenSLRender::Engine Realize fail. result=%d", result);
        return;
    }
    result = (*engineObject)->GetInterface(engineObject,SL_IID_ENGINE,&engineEngine);
    if(result != SL_RESULT_SUCCESS){
        LOGE("OpenSLRender::Engine GetInterface fail. result=%d", result);
        return;
    }
    //创建混音器
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine,&outputMixObj,1,mids,mreq);
    if(result != SL_RESULT_SUCCESS){
        LOGE("OpenSLRender::Init CreateOutputMix fail. result=%d", result);
        return;
    }
    result = (*outputMixObj)->Realize(outputMixObj,SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS){
        LOGE("OpenSLRender::OutMix Realize fail. result=%d", result);
        return;
    }
    result = (*outputMixObj)->GetInterface(outputMixObj,SL_IID_ENVIRONMENTALREVERB,&slEnvironmentalReverbItf);
    if(result != SL_RESULT_SUCCESS){
        LOGE("OpenSLRender::OutMix GetInterface fail. result=%d", result);
        return;
    }
    //创建播放器
    SLDataLocator_AndroidSimpleBufferQueue  androidSimpleBufferQueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2};
    SLDataFormat_PCM pcm = {SL_DATAFORMAT_PCM,
                            (SLuint32)2,
                            SL_SAMPLINGRATE_44_1,
                            SL_PCMSAMPLEFORMAT_FIXED_16,
                            SL_PCMSAMPLEFORMAT_FIXED_16,
                            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                            SL_BYTEORDER_LITTLEENDIAN};

    SLDataSource audioSrc = {&androidSimpleBufferQueue,&pcm};
    SLDataLocator_OutputMix locatorOutputMix = {SL_DATALOCATOR_OUTPUTMIX,outputMixObj};
    SLDataSink dataSink = {&locatorOutputMix, nullptr};
    SLInterfaceID  ids[3] = {SL_IID_BUFFERQUEUE,SL_IID_EFFECTSEND,SL_IID_VOLUME};
    SLboolean  req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine,&playerObject,&audioSrc,&dataSink,3,ids,req);
    if(result != SL_RESULT_SUCCESS){
        LOGE("OpenSLRender::Init CreateAudioPlayer fail. result=%d", result);
        return;
    }
    result = (*playerObject)->Realize(playerObject,SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS){
        LOGE("OpenSLRender::Player Realize fail. result=%d", result);
        return;
    }
    result = (*playerObject)->GetInterface(playerObject,SL_IID_PLAY,&playItf);
    if(result != SL_RESULT_SUCCESS){
        LOGE("OpenSLRender::Player GetInterface fail. result=%d", result);
        return;
    }
    //声音控制
    result = (*playerObject)->GetInterface(playerObject,SL_IID_VOLUME,&playVolume);
    if(result != SL_RESULT_SUCCESS){
        LOGE("OpenSLRender::Volume GetInterface fail. result=%d", result);
        return;
    }

    //队列
    result = (*playerObject)->GetInterface(playerObject,SL_IID_BUFFERQUEUE,&bufferQueueItf);
    if(result != SL_RESULT_SUCCESS){
        LOGE("OpenSLRender::queue GetInterface fail. result=%d", result);
        return;
    }
    result = (*bufferQueueItf)->RegisterCallback(bufferQueueItf,AudioPlayerCallback,this);
    if(result != SL_RESULT_SUCCESS){
        LOGE("OpenSLRender::queue RegisterCallback fail. result=%d", result);
        return;
    }
    isInitSuccess_ = true;
    LOGI("init success !");
//    (*playVolume)->SetVolumeLevel(playVolume,20 * -50);

    auto func = [&](){
        //startrender
        StartRender();
    };

    thread_ = std::thread(func);

}

void AudioRender::RenderFrame(uint8_t *data, int size) {
    if(playItf && isInitSuccess_){
        //添加到队列中
        if (data != nullptr && size > 0){
            while (GetAudioFrameQueueSize() >= MAX_QUEUE_BUFFER_SIZE){
                std::this_thread::sleep_for(std::chrono::milliseconds(15));
            }
            std::unique_lock<std::mutex> lock(mutex_);
            AudioFrame *audioFrame = new AudioFrame(data, size);
            audioFrameQueue_.push(audioFrame);
            cond_.notify_all();
            lock.unlock();
        }
    }
}

void AudioRender::UnInit() {
    if(playItf){
        (*playItf)->SetPlayState(playItf,SL_PLAYSTATE_STOPPED);
        playItf = nullptr;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    exit_ = true;
    isInitSuccess_ = false;
    cond_.notify_all();
    lock.unlock();

    if(playerObject){
        (*playerObject)->Destroy(playerObject);
        playerObject = nullptr;
    }

    if(outputMixObj){
        (*outputMixObj)->Destroy(outputMixObj);
        outputMixObj = nullptr;
    }
    if(engineObject){
        (*engineObject)->Destroy(engineObject);
        engineObject = nullptr;
        engineEngine = nullptr;
    }

    //中止线程
    lock.lock();
    for (int i = 0; i < audioFrameQueue_.size(); ++i) {
        auto frame = audioFrameQueue_.front();
        audioFrameQueue_.pop();
        delete frame;
    }
    lock.unlock();

    if(thread_.joinable()){
        thread_.join();
    }

}

void AudioRender::StartRender() {
    if(!isInitSuccess_){
        return;
    }
    while (GetAudioFrameQueueSize() < MAX_QUEUE_BUFFER_SIZE && !exit_){
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait_for(lock,std::chrono::milliseconds(10));
        lock.unlock();
    }
    (*playItf)->SetPlayState(playItf,SL_PLAYSTATE_PLAYING);

    //处理收到音频数据的回调
    HandleAudoFrameQueue();
}

int AudioRender::GetAudioFrameQueueSize() {
    std::unique_lock<std::mutex> lock(mutex_);
    return audioFrameQueue_.size();
}

void AudioRender::AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context) {
    AudioRender *videoRender = static_cast<AudioRender *>(context);
    videoRender->HandleAudoFrameQueue();
}

void AudioRender::HandleAudoFrameQueue() {
    if(!playItf){
        return;
    }
    while (GetAudioFrameQueueSize() < MAX_QUEUE_BUFFER_SIZE && !exit_){
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait_for(lock,std::chrono::milliseconds(10));
    }
    std::unique_lock<std::mutex> lock(mutex_);
    auto audoFrame = audioFrameQueue_.front();
    if(audoFrame != nullptr && playItf){
        SLresult  result = (*bufferQueueItf)->Enqueue(bufferQueueItf,audoFrame->data,(SLuint32)audoFrame->dataSize);
        if(result == SL_RESULT_SUCCESS){
            //update audioframe
            audioFrameQueue_.pop();
            delete audoFrame;
        }
    }
    lock.unlock();

}