#include "VideoRender.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <log/log.h>
#include "gl/GLUtils.h"

static char vShaderStr[] =
        "#version 300 es\n"
        "layout(location = 0) in vec4 a_position;\n"
        "layout(location = 1) in vec2 a_texCoord;\n"
        "uniform mat4 u_MVPMatrix;\n"
        "out vec2 v_texCoord;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = u_MVPMatrix * a_position;\n"
        "    v_texCoord = a_texCoord;\n"
        "}";

static char fShaderStr[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_TextureMap;//采样器\n"
        "void main()\n"
        "{\n"
        "    outColor = texture(s_TextureMap, v_texCoord);\n"
        "}";

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

GLushort indices[] = {0, 1, 2, 0, 2, 3};

/*OpenGlVideoRender & OpenGlVideoRender::GetInstance() {
    return instance_;
}*/

//OpenGlVideoRender::OpenGlVideoRender() {
//
//}

//OpenGlVideoRender::~OpenGlVideoRender() {
//
//}

void OpenGlVideoRender::Init(int videoWidth, int videoHeight, int *dstSize) {
//    m_FrameIndex = 0;
    UpdateMVPMatrix(0, 0, 1.0f, 1.0f);
}

void OpenGlVideoRender::RenderFrame() {

}

void OpenGlVideoRender::UnInit() {

}

void OpenGlVideoRender::OnSurfaceCreated() {
    //创建openGL的相关内容
    m_ProgramObj = gl::CreateProgram(vShaderStr, fShaderStr);
    if (!m_ProgramObj) {
        LOGE("OpenGLRender::OnSurfaceCreated create program fail");
        return;
    }

    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);

    glBindVertexArray(GL_NONE);

    UpdateMVPMatrix(0, 0, 1.0f, 1.0f);
}

void OpenGlVideoRender::OnSurfaceChanged(int width, int height) {
//    m_ScreenSize.x = w;
//    m_ScreenSize.y = h;
    glViewport(0, 0, width, height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void OpenGlVideoRender::OnDrawFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
//    if(m_ProgramObj == GL_NONE || m_TextureId == GL_NONE || m_RenderImage.ppPlane[0] == nullptr) return;
//    LOGCATE("OpenGLRender::OnDrawFrame [w, h]=[%d, %d]", m_RenderImage.width, m_RenderImage.height);
//    m_FrameIndex++;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    std::unique_lock<std::mutex> lock(mutex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderImage_.width, renderImage_.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, renderImage_.ppPlane[0]);
    lock.unlock();

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glUseProgram (m_ProgramObj);
    glBindVertexArray(m_VaoId);

    gl::setMat4(m_ProgramObj, "u_MVPMatrix", m_MVPMatrix);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    gl::setFloat(m_ProgramObj, "s_TextureMap", 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
}


void OpenGlVideoRender::UpdateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY) {
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(M_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(M_PI / 180.0f * angleY);
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scaleX, scaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    m_MVPMatrix = Projection * View * Model;
}