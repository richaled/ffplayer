#include <jni.h>
#include "jniutils/JNIKey.h"
#include "jniutils/nativeref/NativeObjectRef.h"
#include "jniutils/JniUtils.h"
#include "player.h"

using namespace learn;
using namespace player;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeCreateMediaPlayer(JNIEnv *env, jclass clazz) {
    std::shared_ptr<Player> player = std::shared_ptr<Player>(
            new Player());
    return JniUtils::NewRefWrapJlong(player, kPlayer2);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_MediaPlayer2_nativeStart(JNIEnv *env, jobject thiz) {
    std::shared_ptr<Player> player = JniUtils::CopyRefGet<Player>(
            env, thiz, kPlayer2);
    player->Start();
}


