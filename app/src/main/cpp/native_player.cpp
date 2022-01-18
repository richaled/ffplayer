#include <jni.h>
#include "player/VideoPlayer.h"
#include "jniutils/JNIKey.h"
#include "jniutils/nativeref/NativeObjectRef.h"
#include "jniutils/JniUtils.h"

using namespace learn;



extern "C"
JNIEXPORT jlong JNICALL
Java_com_test_ffmpegdemo_VideoPlayer_nativeCreateVideoPlayer(JNIEnv *env, jclass clazz) {
    std::shared_ptr<VideoPlayer> player = std::shared_ptr<VideoPlayer>(
            new VideoPlayer());
    return JniUtils::NewRefWrapJlong(player, kPlayer);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_VideoPlayer_nativeSetCallback(JNIEnv *env, jobject thiz) {
    std::shared_ptr<VideoPlayer> player = JniUtils::CopyRefGet<VideoPlayer>(
            env, thiz, kPlayer);
    auto globalRef = JniUtils::LocalRawObjectToGlobalRefObject(thiz, env);
    player->SetStateCallback([globalRef](const int state){
        JNIEnv *env = JniUtils::AttachThreadLocalEnv();
        jobject javaObj = globalRef.get();
        jmethodID mid = env->GetMethodID(env->GetObjectClass(javaObj), "onStateChange", "(I)V");
        env->CallVoidMethod(javaObj, mid, state);

    });
    player->SetProgressCallback([globalRef](const float progress, const float total){
        JNIEnv *env = JniUtils::AttachThreadLocalEnv();
        jobject javaObj = globalRef.get();
        jmethodID mid = env->GetMethodID(env->GetObjectClass(javaObj), "onProgress", "(FF)V");
        env->CallVoidMethod(javaObj, mid, progress,total);
    });

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_test_ffmpegdemo_VideoPlayer_nativePrepare(JNIEnv *env, jobject thiz, jstring url,jobject playerRender) {
    std::shared_ptr<VideoPlayer> player = JniUtils::CopyRefGet<VideoPlayer>(
            env, thiz, kPlayer);
    if (player == nullptr || url == nullptr){
        return -1;
    }
    std::shared_ptr<OpenGlVideoRender> render = JniUtils::CopyRefGet<OpenGlVideoRender>(
            env, playerRender, kGlRender);
    player->Init(JniUtils::JavaStringToString(url,env),render);
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_VideoPlayer_nativePlay(JNIEnv *env, jobject thiz) {
    std::shared_ptr<VideoPlayer> player = JniUtils::CopyRefGet<VideoPlayer>(
            env, thiz, kPlayer);
    player->Play();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_VideoPlayer_nativePause(JNIEnv *env, jobject thiz) {
    std::shared_ptr<VideoPlayer> player = JniUtils::CopyRefGet<VideoPlayer>(
            env, thiz, kPlayer);
    player->Pause();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_VideoPlayer_nativeStop(JNIEnv *env, jobject thiz) {
    std::shared_ptr<VideoPlayer> player = JniUtils::CopyRefGet<VideoPlayer>(
            env, thiz, kPlayer);
    player->Stop();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_VideoPlayer_nativeSeekTo(JNIEnv *env, jobject thiz, jfloat time) {
    std::shared_ptr<VideoPlayer> player = JniUtils::CopyRefGet<VideoPlayer>(
            env, thiz, kPlayer);
    if(player){
        player->SeekTo(time);
    }
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_com_test_ffmpegdemo_VideoPlayer_nativeGetDuration(JNIEnv *env, jobject thiz) {
    std::shared_ptr<VideoPlayer> player = JniUtils::CopyRefGet<VideoPlayer>(
            env, thiz, kPlayer);
    if(player){
        return player->GetDuration();
    }
    return -1;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_VideoPlayer_nativeRelease(JNIEnv *env, jobject thiz) {
    std::shared_ptr<VideoPlayer> player = JniUtils::CopyRefGet<VideoPlayer>(
            env, thiz, kPlayer);
    player->UnInit();
}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_test_ffmpegdemo_PlayerRender_nativeCreateRender(JNIEnv *env, jclass clazz) {
    std::shared_ptr<OpenGlVideoRender> render = std::shared_ptr<OpenGlVideoRender>(
            new OpenGlVideoRender());
    return JniUtils::NewRefWrapJlong(render, kGlRender);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_PlayerRender_nativeOnSurfaceCreated(JNIEnv *env, jobject thiz) {
    std::shared_ptr<OpenGlVideoRender> render = JniUtils::CopyRefGet<OpenGlVideoRender>(
            env, thiz, kGlRender);
    render->OnSurfaceCreated();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_PlayerRender_nativeonSurfaceChanged(JNIEnv *env, jobject thiz, jint width,
                                                             jint height) {
    std::shared_ptr<OpenGlVideoRender> render = JniUtils::CopyRefGet<OpenGlVideoRender>(
            env, thiz, kGlRender);
    render->OnSurfaceChanged(width,height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_PlayerRender_nativeonDrawFrame(JNIEnv *env, jobject thiz) {
    std::shared_ptr<OpenGlVideoRender> render = JniUtils::CopyRefGet<OpenGlVideoRender>(
            env, thiz, kGlRender);
    render->OnDrawFrame();
}
