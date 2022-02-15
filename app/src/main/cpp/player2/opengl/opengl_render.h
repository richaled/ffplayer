//
// Created by richard on 2022/2/14.
//

#pragma once

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "gl_utils.h"

namespace player {

    enum RenderFrame {
        SQUARE = 0,
        // 按原比例显示,上下或者左右留黑
        FIT,
        // 铺满显示view,同时画面裁剪
        CROP
    };

    enum TextureType {
        TEXTURE_OES = 0, //android eos 纹理
        TEXTURE_2D
    };

    class OpenglRender {
    public:
        OpenglRender(const int width, const int height, const char *vertex, const char *fragment);

        virtual ~OpenglRender();
        void SetRenderType(const TextureType textureType = TextureType::TEXTURE_2D);

        void ActiveProgram();
        void ProcessImage(GLuint texture_id, const GLfloat* vertex_coordinate, const GLfloat* texture_coordinate);
        void ProcessImage(GLuint texture_id, const GLfloat* vertex_coordinate, const GLfloat* texture_coordinate, GLfloat* texture_matrix);


    protected:

    private:
        void CreateGLProgram(const char *vertex, const char *fragment);
    private:
        GLuint program_;
        int width_;
        int height_;
        TextureType textureType_;
    };
}


