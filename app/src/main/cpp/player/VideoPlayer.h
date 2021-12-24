#pragma once
#include "render/VideoRender.h"
#include "decoder/VideoDecoder.h"
#include "string"
#include "IPlayer.h"
/**
 * 播放器，组织opengl的render和ffmpeg的解码
 */


class VideoPlayer :public IPlayer {
public:
    VideoPlayer(){};
    ~VideoPlayer() ;
    void Init(const std::string &url) override;
    void UnInit() override;
    void Play()  override;
    void Pause()  override;
    void Stop()  override;
    void SeekTo(float position) override ;
    float GetDuration();

private:
    std::shared_ptr<VideoDecoder> videoDecoder_;
    std::shared_ptr<VideoRender> videoRender_;
};


