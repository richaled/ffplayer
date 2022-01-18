#pragma once
#include "render/VideoRender.h"
#include "decoder/VideoDecoder.h"
#include "decoder/AudioDecoder.h"

#include "string"
#include "IPlayer.h"
/**
 * 播放器，组织opengl的render和ffmpeg的解码
 */


class VideoPlayer :public IPlayer {
public:
    VideoPlayer(){};
    ~VideoPlayer() ;
    void Init(const std::string &url, const std::shared_ptr<VideoRender> &render) override;
    void UnInit() override;
    void Play()  override;
    void Pause()  override;
    void Stop()  override;
    void SeekTo(float position) override ;
    float GetDuration() override;

    void SetStateCallback(const StateCallback &stateCallback);
    void SetProgressCallback(const ProgressCallback &progressCallback);

private:
    std::shared_ptr<VideoDecoder> videoDecoder_;
    std::shared_ptr<AudioDecoder> audioDecoder_;
    StateCallback stateCallback_;
    ProgressCallback progressCallback_;
};


