#pragma once


class GlRender{
public:
    virtual void OnSurfaceCreated() = 0;
    virtual void OnSurfaceChanged(int width, int height)  = 0;
    virtual void OnDrawFrame()  = 0;
};

class VideoRender {
public:
    virtual void Init(int videoWidth, int videoHeight, int *dstSize) = 0;
    virtual void RenderFrame(NativeImage *nativeImage) = 0;
    virtual void UnInit() = 0;
};

class GLVideoRender : public GlRender , public VideoRender{

};