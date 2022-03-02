#include <log/log.h>
#include "yuv_render.h"
#include "gl_utils.h"
#include "gl_info.h"

namespace player{
    const char *YUV_FRAME_FRAGMENT_SHADER =
            "varying highp vec2 textureCoordinate;\n"
            "uniform sampler2D texture_y;\n"
            "uniform sampler2D texture_u;\n"
            "uniform sampler2D texture_v;\n"
            "void main() { \n"
            "   highp float y = texture2D(texture_y, textureCoordinate).r;\n"
            "   highp float u = texture2D(texture_u, textureCoordinate).r - 0.5;\n"
            "   highp float v = texture2D(texture_v, textureCoordinate).r - 0.5;\n"
            "   highp float r = y + 1.402 * v;\n"
            "   highp float g = y - 0.344 * u - 0.714 * v;\n"
            "   highp float b = y + 1.772 * u;\n"
            "   gl_FragColor = vec4(r,g,b,1.0);\n"
            "}\n";

    const char* NV21_FRAGMENT_SHADER =
            "varying highp vec2 textureCoordinate;\n"
            "uniform sampler2D texture_y;\n"
            "uniform sampler2D texture_u;\n"
            "void main() {\n"
            "   float y = texture2D(texture_y, textureCoordinate).r;\n"
            "   vec4 uv = texture2D(texture_u, textureCoordinate);\n"
            "   float u = uv.r - 0.5;\n"
            "   float v = uv.a - 0.5;\n"
            "   float r = y + 1.402 * v;\n"
            "   float g = y - 0.344 * u - 0.714 * v;\n"
            "   float b = y + 1.772 * u;\n"
            "   gl_FragColor = vec4(r, g, b, 1.0);\n"
            "}\n";



    YuvRender::YuvRender() {
    }

    YuvRender::~YuvRender() {
        Destory();
    }

    void YuvRender::Destory() {
        for (int i = 0; i < 3; i++) {
            GLuint texture = textures_[i];
            glDeleteTextures(1, &texture);
            textures_[i] = 0;
        }
        if (textureId_ != 0) {
            glDeleteTextures(1, &textureId_);
            textureId_ = 0;
        }

        if (frameBufferId_ != 0) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &frameBufferId_);
            frameBufferId_ = 0;
        }

        if (program_ != 0) {
            glDeleteProgram(program_);
            program_ = 0;
        }
    }

    void YuvRender::CreateGLProgram(const int width, const int height, const char *fragment) {
        program_ = CreateProgram(DEFAULT_VERTEX_SHADER,fragment);

        vertexPosLocation_ = glGetAttribLocation(program_,"position");
        vertexTextureLocation_ = glGetAttribLocation(program_,"inputTextureCoordinate");
        textureMatrixLocation_ = glGetAttribLocation(program_,"textureMatrix");
//        glUseProgram(program_);
        uniformSamplers_[0] = glGetUniformLocation(program_, "texture_y");
        uniformSamplers_[1] = glGetUniformLocation(program_, "texture_u");
        uniformSamplers_[2] = glGetUniformLocation(program_, "texture_v");

        if(textureId_ != 0){
            glDeleteTextures(1,&textureId_);
        }
        if(frameBufferId_ != 0){
            glDeleteFramebuffers(1,&frameBufferId_);
        }
        glGenTextures(3,textures_);
        for (int i = 0; i < 3; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures_[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, GL_NONE);
        }

        glGenFramebuffers(1,&frameBufferId_);
        glGenTextures(1,&textureId_);
        glBindTexture(GL_TEXTURE_2D,textureId_);
        glBindFramebuffer(GL_FRAMEBUFFER,frameBufferId_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId_, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint YuvRender::DrawFrame(AVFrame *frame, const GLfloat *matrix, const GLfloat *vertex_coordinate, const GLfloat *texture_coordinate) {
        int width = frame->width;
        int height = frame->height;
        if(program_ == 0){
            switch (frame->format) {
                case AV_PIX_FMT_YUV420P:{
                    CreateGLProgram(width,height,YUV_FRAME_FRAGMENT_SHADER);
                    break;
                }
                case AV_PIX_FMT_NV12:{
                    CreateGLProgram(width,height,NV21_FRAGMENT_SHADER);
                    break;
                }
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER,frameBufferId_);
        //如果宽度不是16的倍数
        if (width % 16 != 0) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glViewport(0,0,width,height);
        glUseProgram(program_);
        switch (frame->format) {
            case AV_PIX_FMT_YUV420P:{
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textures_[0]);
                glUniform1i(uniformSamplers_[0], 0);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, frame->linesize[0], frame->height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame->data[0]);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, textures_[1]);
                glUniform1i(uniformSamplers_[1], 1);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, frame->linesize[1], frame->height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame->data[1]);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, textures_[2]);
                glUniform1i(uniformSamplers_[2], 2);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, frame->linesize[2], frame->height / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame->data[2]);

                break;
            }
            case AV_PIX_FMT_NV12:{
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textures_[0]);
                glUniform1i(uniformSamplers_[0], 0);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, frame->linesize[0], frame->height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame->data[0]);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, textures_[1]);
                glUniform1i(uniformSamplers_[1], 1);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, frame->linesize[1] / 2, frame->height / 2, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, frame->data[1]);
                break;
            }
            default:
                LOGE("yuv frame :%d is not support !!!",frame->format);
                break;
        }

        glVertexAttribPointer(vertexPosLocation_, 2, GL_FLOAT, GL_FALSE, 0, vertex_coordinate);
        glEnableVertexAttribArray(vertexPosLocation_);
        glVertexAttribPointer(vertexTextureLocation_, 2, GL_FLOAT, GL_FALSE, 0, texture_coordinate);
        glEnableVertexAttribArray(vertexTextureLocation_);
//        glUniformMatrix4fv(textureMatrixLocation_, 1, GL_FALSE, matrix);
        for (int i = 0; i < 3; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures_[i]);
            glUniform1i(uniformSamplers_[i], i);
        }
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(vertexPosLocation_);
        glDisableVertexAttribArray(vertexTextureLocation_);

        glBindTexture(GL_TEXTURE_2D,0);
        glBindFramebuffer(GL_FRAMEBUFFER,0);

        return textureId_;
    }


}
