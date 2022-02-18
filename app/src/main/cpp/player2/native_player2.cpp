#include <jni.h>
#include <log/log.h>
#include "jniutils/JNIKey.h"
#include "jniutils/nativeref/NativeObjectRef.h"
#include "jniutils/JniUtils.h"
#include "android_player.h"

#include <android/native_window_jni.h>


using namespace learn;
using namespace player;
using namespace JniUtils;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeCreateMediaPlayer(JNIEnv *env, jclass clazz) {
    std::shared_ptr<AndroidPlayer> player = std::shared_ptr<AndroidPlayer>(
            new AndroidPlayer());
    return JniUtils::NewRefWrapJlong(player, kPlayer2);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeSetMediaClipSource(JNIEnv *env, jobject thiz,
                                                                      jobjectArray media_clips) {
    std::shared_ptr<AndroidPlayer> player = JniUtils::CopyRefGet<AndroidPlayer>(
            env, thiz, kPlayer2);
    //convert to
    std::vector<LocalJniObject> clipsObjs = JavaObjectArrayToVector(media_clips,env);
    jclass clipCls = env->FindClass("com/test/ffmpegdemo/player/MediaClip");
    jclass timeRangeCls = env->FindClass("com/test/ffmpegdemo/player/MediaClip$TimeRange");
    if(clipCls == nullptr || timeRangeCls == nullptr){
        LOGI("fail get mediaclip or timerange class");
        return;
    }
    jfieldID pathId = env->GetFieldID(clipCls,"path","Ljava/lang/String;");
    jfieldID typeId = env->GetFieldID(clipCls,"type","I");
    jfieldID timeRangeId = env->GetFieldID(clipCls,"timeRange", "Lcom/test/ffmpegdemo/player/MediaClip$TimeRange;");
    jfieldID startTimeId = env->GetFieldID(timeRangeCls,"startTime","J");
    jfieldID endTimeId = env->GetFieldID(timeRangeCls,"endTime","J");
    std::vector<MediaClip> mediaClips(clipsObjs.size());
    for (int i = 0; i < clipsObjs.size(); ++i) {
        jobject clip = clipsObjs[i].get();
        jstring path = (jstring)env->GetObjectField(clip,pathId);
        jint type = env->GetIntField(clip,typeId);
        jobject timeRange = env->GetObjectField(clip,timeRangeId);
        jlong startTime = env->GetLongField(timeRange,startTimeId);
        jlong endTime = env->GetLongField(timeRange,endTimeId);

        MediaClip  mediaClip;
        mediaClip.file_name = JavaStringToString(path,env);
        mediaClip.type = type;
        mediaClip.start_time = startTime;
        mediaClip.end_time = endTime;
        mediaClips[i] = mediaClip;
    }
    MediaInfo mediaInfo;
    mediaInfo.clips = mediaClips;
    player->SetMediaSources(mediaInfo);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativePrepare(JNIEnv *env, jobject thiz, jobject options_) {
    std::shared_ptr<AndroidPlayer> player = JniUtils::CopyRefGet<AndroidPlayer>(
            env, thiz, kPlayer2);
    //convert to c++ options
    std::shared_ptr<Options> options = JniUtils::CopyRefGet<Options>(env, options_, kOptions);
    player->Prepare(*options);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeAddClip(JNIEnv *env, jobject thiz,
                                                           jobject media_clip) {
    std::shared_ptr<AndroidPlayer> player = JniUtils::CopyRefGet<AndroidPlayer>(
            env, thiz, kPlayer2);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativePlay(JNIEnv *env, jobject thiz) {
    std::shared_ptr<AndroidPlayer> player = JniUtils::CopyRefGet<AndroidPlayer>(
            env, thiz, kPlayer2);
    player->Start();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeStop(JNIEnv *env, jobject thiz) {
    std::shared_ptr<AndroidPlayer> player = JniUtils::CopyRefGet<AndroidPlayer>(
            env, thiz, kPlayer2);
    player->Stop();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeResume(JNIEnv *env, jobject thiz) {
    std::shared_ptr<AndroidPlayer> player = JniUtils::CopyRefGet<AndroidPlayer>(
            env, thiz, kPlayer2);
    player->Resume();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativePause(JNIEnv *env, jobject thiz) {
    std::shared_ptr<AndroidPlayer> player = JniUtils::CopyRefGet<AndroidPlayer>(
            env, thiz, kPlayer2);
    player->Pause();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeSeek(JNIEnv *env, jobject thiz, jlong time_ms) {
    std::shared_ptr<AndroidPlayer> player = JniUtils::CopyRefGet<AndroidPlayer>(
            env, thiz, kPlayer2);
    player->SeekTo(time_ms);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeIsPrepare(JNIEnv *env, jobject thiz) {
    std::shared_ptr<AndroidPlayer> player = JniUtils::CopyRefGet<AndroidPlayer>(
            env, thiz, kPlayer2);
    return player->IsPrepare() ? JNI_TRUE : JNI_FALSE;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeCreateSurface(JNIEnv *env, jobject thiz,
                                                                 jobject surface) {
    std::shared_ptr<AndroidPlayer> player = JniUtils::CopyRefGet<AndroidPlayer>(
            env, thiz, kPlayer2);
    ANativeWindow *window = ANativeWindow_fromSurface(env, (jobject)surface);
    player->CreateSurfaceWindow(window);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeSurfaceSizeChange(JNIEnv *env, jobject thiz,
                                                                     jint width, jint height) {
    std::shared_ptr<AndroidPlayer> player = JniUtils::CopyRefGet<AndroidPlayer>(
            env, thiz, kPlayer2);
    player->SurfaceWindowSizeChange(width,height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeDestorySurface(JNIEnv *env, jobject thiz) {
    std::shared_ptr<AndroidPlayer> player = JniUtils::CopyRefGet<AndroidPlayer>(
            env, thiz, kPlayer2);
    player->DestorySurfaceWindow();
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeIsPlaying(JNIEnv *env, jobject thiz) {
    std::shared_ptr<AndroidPlayer> player = JniUtils::CopyRefGet<AndroidPlayer>(
            env, thiz, kPlayer2);
    if(player){
        return player->IsPlaying() == JNI_TRUE;
    }
    return JNI_FALSE;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeGetDuration(JNIEnv *env, jobject thiz) {
    std::shared_ptr<AndroidPlayer> player = JniUtils::CopyRefGet<AndroidPlayer>(
            env, thiz, kPlayer2);
    if(player){
        return player->GetDuration();
    }
    return 0l;
}


