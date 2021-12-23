#pragma once

#include <GLES3/gl3.h>
#include "ImageDef.h"
#include "mutex"
#include "glm/glm.hpp"

class VideoRender{
public:
    virtual ~VideoRender(){};
    virtual void Init(int videoWidth, int videoHeight, int *dstSize) = 0;
    virtual void RenderFrame() = 0;
    virtual void UnInit() = 0;
};

class OpenGlVideoRender : public VideoRender{
public:
    OpenGlVideoRender();
    ~OpenGlVideoRender();
    virtual void Init(int videoWidth, int videoHeight, int *dstSize) override ;
    virtual void RenderFrame() override ;
    virtual void UnInit() override ;

    void OnSurfaceCreated();
    void OnSurfaceChanged(int width, int height);
    void OnDrawFrame();

    void UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY);
private:
    NativeImage renderImage_;
    GLuint m_ProgramObj = GL_NONE;
    GLuint m_TextureId;
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    std::mutex mutex_;
    glm::mat4 m_MVPMatrix;
};