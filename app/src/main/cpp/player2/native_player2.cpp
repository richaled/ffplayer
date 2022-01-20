#include <jni.h>
#include <log/log.h>
#include "jniutils/JNIKey.h"
#include "jniutils/nativeref/NativeObjectRef.h"
#include "jniutils/JniUtils.h"
#include "player.h"

using namespace learn;
using namespace player;
using namespace JniUtils;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeCreateMediaPlayer(JNIEnv *env, jclass clazz) {
    std::shared_ptr<Player> player = std::shared_ptr<Player>(
            new Player());
    return JniUtils::NewRefWrapJlong(player, kPlayer2);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativePrepare(JNIEnv *env, jobject thiz, jobjectArray clips) {
    std::shared_ptr<Player> player = JniUtils::CopyRefGet<Player>(
            env, thiz, kPlayer2);
    //convert to
    std::vector<LocalJniObject> clipsObjs = JavaObjectArrayToVector(clips,env);
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
    mediaInfo.hardWare = true;
    player->Prepare(mediaInfo);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeIsPrepare(JNIEnv *env, jobject thiz) {
    std::shared_ptr<Player> player = JniUtils::CopyRefGet<Player>(
            env, thiz, kPlayer2);
    return player->IsPrepare() ? JNI_TRUE : JNI_FALSE;
}


