#pragma once

#include <GLES3/gl3.h>
#include <string>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"

namespace player{
     GLuint LoadShader(GLenum shaderType, const char *pSource);

     GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource,
                                GLuint &vertexShaderHandle,
                                GLuint &fragShaderHandle);

     GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource);

     GLuint CreateProgramWithFeedback(
            const char *pVertexShaderSource,
            const char *pFragShaderSource,
            GLuint &vertexShaderHandle,
            GLuint &fragShaderHandle,
            const GLchar **varying,
            int varyingCount);

     void DeleteProgram(GLuint &program);

     void Link(GLuint program);

     void CheckGLError(const char *pGLOperation);

     void setBool(GLuint programId, const std::string &name, bool value);

     void setInt(GLuint programId, const std::string &name, int value);

     void setFloat(GLuint programId, const std::string &name, float value);

     void setVec2(GLuint programId, const std::string &name, const glm::vec2 &value);

     void setVec2(GLuint programId, const std::string &name, float x, float y);

     void setVec3(GLuint programId, const std::string &name, const glm::vec3 &value);

     void setVec3(GLuint programId, const std::string &name, float x, float y, float z);

     void setVec4(GLuint programId, const std::string &name, const glm::vec4 &value);

     void setVec4(GLuint programId, const std::string &name, float x, float y, float z, float w);

     void setMat2(GLuint programId, const std::string &name, const glm::mat2 &mat);

     void setMat3(GLuint programId, const std::string &name, const glm::mat3 &mat);

     void setMat4(GLuint programId, const std::string &name, const glm::mat4 &mat);

     glm::vec3 texCoordToVertexCoord(glm::vec2 texCoord);

}

