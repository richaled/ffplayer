#pragma once

class VideoRender{
public:
//    VideoRender(){};
//    virtual ~VideoRender();
    virtual void Init(int videoWidth, int videoHeight, int *dstSize) = 0;
    virtual void RenderFrame() = 0;
    virtual void UnInit() = 0;
};