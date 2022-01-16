#pragma once

#include <GLES3/gl3.h>
#include "ImageDef.h"
#include "mutex"
#include "gl/GLUtils.h"
#include "Render.h"

#define TEXTURE_NUM 3

class OpenGlVideoRender: public GLVideoRender ,public std::enable_shared_from_this<OpenGlVideoRender>{
public:
//    static OpenGlVideoRender& GetInstance();
    OpenGlVideoRender(){};
    ~OpenGlVideoRender();

    void Init(int videoWidth, int videoHeight, int *dstSize) override ;
    void RenderFrame(NativeImage *nativeImage) override ;
    void UnInit() override ;

    void OnSurfaceCreated();
    void OnSurfaceChanged(int width, int height);
    void OnDrawFrame();

    void UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY);

    std::shared_ptr<OpenGlVideoRender> getRender(){
        return shared_from_this();
    }
private:

private:
    static OpenGlVideoRender instance_;
    NativeImage renderImage_;
    GLuint m_ProgramObj = GL_NONE;
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    std::mutex mutex_;
    glm::mat4 m_MVPMatrix;
    long m_FrameIndex = 0;

    GLuint m_TextureIds[3];
};