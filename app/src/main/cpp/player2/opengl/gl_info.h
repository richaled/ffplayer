//
// Created by richard on 2022/2/14.
//
#pragma once

namespace player{
    static const float DEFAULT_VERTEX_COORDINATE[] = {
            -1.F, -1.F,
            1.F, -1.F,
            -1.F, 1.F,
            1.F, 1.F
    };

    static const float DEFAULT_TEXTURE_COORDINATE[] = {
            0.F, 0.F,
            1.F, 0.F,
            0.F, 1.F,
            1.F, 1.F
    };


    // 默认带matrix的顶点shader
    static const char* DEFAULT_VERTEX_MATRIX_SHADER =
            "attribute vec4 position;                                                               \n"
            "attribute vec4 inputTextureCoordinate;                                                 \n"
            "varying vec2 textureCoordinate;                                                        \n"
            "uniform highp mat4 textureMatrix;                                                      \n"
            "void main() {                                                                          \n"
            "  textureCoordinate = (textureMatrix * inputTextureCoordinate).xy;                     \n"
            "  gl_Position = position;                                                              \n"
            "}                                                                                      \n";

    // 默认OES fragment shader
    static const char* DEFAULT_OES_FRAGMENT_SHADER =
            "#extension GL_OES_EGL_image_external : require                                         \n"
            "precision mediump float;                                                               \n"
            "uniform samplerExternalOES inputImageTexture;                                              \n"
            "varying vec2 textureCoordinate;                                                        \n"
            "void main() {                                                                          \n"
            "  gl_FragColor = texture2D(inputImageTexture, textureCoordinate);                          \n"
            "}                                                                                      \n";

    // 默认顶点shader
    static const char* DEFAULT_VERTEX_SHADER =
            "#ifdef GL_ES                                                                           \n"
            "precision highp float;                                                                 \n"
            "#endif                                                                                 \n"
            "attribute vec4 position;                                                               \n"
            "attribute vec4 inputTextureCoordinate;                                                 \n"
            "varying vec2 textureCoordinate;                                                        \n"
            "void main() {                                                                          \n"
            "    gl_Position = position;                                                            \n"
            "    textureCoordinate = inputTextureCoordinate.xy;                                     \n"
            "}                                                                                      \n";

    // 默认fragment shader
    static const char* DEFAULT_FRAGMENT_SHADER =
            "#ifdef GL_ES                                                                           \n"
            "precision highp float;                                                                 \n"
            "#endif                                                                                 \n"
            "varying vec2 textureCoordinate;                                                        \n"
            "uniform sampler2D inputImageTexture;                                                   \n"
            "void main() {                                                                          \n"
            "    gl_FragColor = texture2D(inputImageTexture, textureCoordinate);                    \n"
            "}                                                                                      \n";

}
