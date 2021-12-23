#include <jni.h>
#include "render/VideoRender.h"
#include "jniutils/JNIKey.h"
#include "jniutils/nativeref/NativeObjectRef.h"

using namespace learn;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_test_ffmpegdemo_PlayerRender_nativeCreateVertexObj(JNIEnv *env, jclass clazz) {
    std::shared_ptr<OpenGlVideoRender> render = std::shared_ptr<OpenGlVideoRender>(
            new OpenGlVideoRender());
    return JniUtils::NewRefWrapJlong(render, kPlayerRender);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_PlayerRender_nativeSurfaceCreated(JNIEnv *env, jobject thiz) {
    std::shared_ptr<OpenGlVideoRender> render = JniUtils::CopyRefGet<OpenGlVideoRender>(
            env, thiz, kPlayerRender);
    render->OnSurfaceCreated();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_PlayerRender_nativeSurfaceChanged(JNIEnv *env, jobject thiz, jint width,
                                                           jint height) {
    std::shared_ptr<OpenGlVideoRender> render = JniUtils::CopyRefGet<OpenGlVideoRender>(
            env, thiz, kPlayerRender);
    render->OnSurfaceChanged(width,height);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_PlayerRender_nativeDrawFrame(JNIEnv *env, jobject thiz) {
    std::shared_ptr<OpenGlVideoRender> render = JniUtils::CopyRefGet<OpenGlVideoRender>(
            env, thiz, kPlayerRender);
    render->OnDrawFrame();
}