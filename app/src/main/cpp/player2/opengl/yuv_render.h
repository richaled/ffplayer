//
// Created by richard on 2022/2/14.
//

#pragma once

/**
 * 直接渲染yuv
 */
#include <GLES3/gl3.h>

extern "C" {
#include "libavcodec/avcodec.h"
};

namespace player {

    class YuvRender {
    public:
        YuvRender();
        ~YuvRender();
        virtual GLuint DrawFrame(AVFrame* frame, const GLfloat* matrix,
                                 const GLfloat* vertex_coordinate, const GLfloat* texture_coordinate);

    private:
        void CreateGLProgram(const int width, const int height, const char *fragment);
        void Destory();

    private:
        GLuint program_;
        GLuint vertexPosLocation_;
        GLuint vertexTextureLocation_;
        GLuint textureMatrixLocation_;
        GLint uniformSamplers_[3];
        GLuint textureId_;
        GLuint frameBufferId_;
        GLuint textures_[3];

    };
}



