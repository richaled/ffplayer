#include "opengl_render.h"
#include <log/log.h>
#include <GLES2/gl2ext.h>

namespace player{

    GLfloat verticesCoords[] = {
            -1.0f, 1.0f, 0.0f,  // Position 0
            -1.0f, -1.0f, 0.0f,  // Position 1
            1.0f, -1.0f, 0.0f,  // Position 2
            1.0f, 1.0f, 0.0f,  // Position 3
    };

    GLfloat textureCoords[] = {
            0.0f, 0.0f,        // TexCoord 0
            0.0f, 1.0f,        // TexCoord 1
            1.0f, 1.0f,        // TexCoord 2
            1.0f, 0.0f         // TexCoord 3
    };

    GLfloat texture_matrix[] = {
            1.0f,0.0f,0.0f,0.0f,
            0.0f,1.0f,0.0f,0.0f,
            0.0f,0.0f,1.0f,0.0f,
            0.0f,0.0f,0.0f,1.0f
    };

    OpenglRender::OpenglRender(const int width, const int height, const char *vertex, const char *fragment)
    :width_(width),height_(height){
        if(vertex && fragment){
            CreateGLProgram(vertex, fragment);
        }
    }

    OpenglRender::~OpenglRender() {

    }

    void OpenglRender::SetRenderType(const TextureType textureType) {
        textureType_ = textureType;
    }

    void OpenglRender::CreateGLProgram(const char *vertex, const char *fragment) {
        program_ = CreateProgram(vertex,fragment);
    }

    void OpenglRender::ActiveProgram() {
        glUseProgram(program_);
    }

    void OpenglRender::ProcessImage(GLuint texture_id, const GLfloat *vertex_coordinate, const GLfloat *texture_coordinate) {
        ProcessImage(texture_id, vertex_coordinate, texture_coordinate, texture_matrix);
    }

    void OpenglRender::ProcessImage(GLuint textureId, const GLfloat* vertex_coordinate, const GLfloat* texture_coordinate, GLfloat* texture_matrix){
//        METHOD
        if(program_ == 0){
            LOGE("program is empty");
            return;
        }
        ActiveProgram();
        if(width_ > 0 && height_ > 0){
            glViewport(0,0,width_,height_);
        }
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //attribute 可用
        auto positionAttribute = static_cast<GLuint>(glGetAttribLocation(program_, "position"));
        glEnableVertexAttribArray(positionAttribute);
        glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), vertex_coordinate);
        auto textureCoordinateAttribute = static_cast<GLuint>(glGetAttribLocation(program_, "inputTextureCoordinate"));
        glEnableVertexAttribArray(textureCoordinateAttribute);
        glVertexAttribPointer(textureCoordinateAttribute, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), texture_coordinate);
        SetUniformMatrix4f(program_,"textureMatrix", 1, texture_matrix);

        glActiveTexture(GL_TEXTURE0);
#if __ANDROID__
        glBindTexture(textureType_ == TEXTURE_OES ? GL_TEXTURE_EXTERNAL_OES : GL_TEXTURE_2D, textureId);
#elif __APPLE__
        glBindTexture(GL_TEXTURE_2D, texture_id);
#endif
        setInt(program_,"inputImageTexture", 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(positionAttribute);
        glDisableVertexAttribArray(textureCoordinateAttribute);
#if __ANDROID__
        glBindTexture(textureType_ == TEXTURE_OES ? GL_TEXTURE_EXTERNAL_OES : GL_TEXTURE_2D, 0);
#elif __APPLE__
        glBindTexture(GL_TEXTURE_2D, 0);
#endif

    }

}