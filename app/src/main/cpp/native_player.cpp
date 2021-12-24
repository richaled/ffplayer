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
Java_com_test_ffmpegdemo_VideoPlayer_nativeSurfaceCreated(JNIEnv *env, jobject thiz) {
    std::shared_ptr<VideoPlayer> player = JniUtils::CopyRefGet<VideoPlayer>(
            env, thiz, kPlayer);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_VideoPlayer_nativeSurfaceChanged(JNIEnv *env, jobject thiz, jint width,
                                                          jint height) {
    std::shared_ptr<VideoPlayer> player = JniUtils::CopyRefGet<VideoPlayer>(
            env, thiz, kPlayer);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_VideoPlayer_nativeDrawFrame(JNIEnv *env, jobject thiz) {
    std::shared_ptr<VideoPlayer> player = JniUtils::CopyRefGet<VideoPlayer>(
            env, thiz, kPlayer);

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_test_ffmpegdemo_VideoPlayer_nativePrepare(JNIEnv *env, jobject thiz, jstring url) {
    std::shared_ptr<VideoPlayer> player = JniUtils::CopyRefGet<VideoPlayer>(
            env, thiz, kPlayer);
    if (player == nullptr || url == nullptr){
        return -1;
    }
    player->Init(JniUtils::JavaStringToString(url,env));
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
Java_com_test_ffmpegdemo_VideoPlayer_nativeRelease(JNIEnv *env, jobject thiz) {
    std::shared_ptr<VideoPlayer> player = JniUtils::CopyRefGet<VideoPlayer>(
            env, thiz, kPlayer);
    player->UnInit();
}