//
// Created by richard on 2022/2/15.
//
#include <jni.h>
#include "jniutils/JNIKey.h"
#include "jniutils/nativeref/NativeObjectRef.h"
#include "jniutils/JniUtils.h"
#include "media_base.h"

using namespace test;
using namespace learn;

extern "C" JNIEXPORT jlong JNICALL
Java_com_test_ffmpegdemo_player_Options_createNativeWrap(JNIEnv *env, jclass type) {
    std::shared_ptr<Options> options(new Options());
    return static_cast<jlong>(reinterpret_cast<intptr_t>(
            JniUtils::NewRefWrap(options, kOptions)));
}

extern "C" JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_Options_setBool(JNIEnv *env, jobject thiz, jint key_, jboolean value_) {
std::shared_ptr<Options> options = JniUtils::CopyRefGet<Options>(env, thiz, kOptions);
auto key = static_cast<OptionKey>(key_);
auto value = static_cast<bool>(value_);
(*options)[key] = value;
}

extern "C" JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_Options_setInt(JNIEnv *env, jobject thiz, jint key_, jint value_) {
std::shared_ptr<Options> options = JniUtils::CopyRefGet<Options>(env, thiz, kOptions);
auto key = static_cast<OptionKey>(key_);
auto value = static_cast<int>(value_);
(*options)[key] = value;
}

extern "C" JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_Options_setInt64(JNIEnv *env, jobject thiz, jint key_, jlong value_) {
std::shared_ptr<Options> options = JniUtils::CopyRefGet<Options>(env, thiz, kOptions);
auto key = static_cast<OptionKey>(key_);
auto value = static_cast<int64_t>(value_);
(*options)[key] = value;
}

extern "C" JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_Options_setDouble(JNIEnv *env, jobject thiz, jint key_, jdouble value_) {
std::shared_ptr<Options> options = JniUtils::CopyRefGet<Options>(env, thiz, kOptions);
auto key = static_cast<OptionKey>(key_);
auto value = static_cast<double>(value_);
(*options)[key] = value;
}

extern "C" JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_Options_setString(JNIEnv *env, jobject thiz, jint key_, jstring value_) {
std::shared_ptr<Options> options = JniUtils::CopyRefGet<Options>(env, thiz, kOptions);
auto key = static_cast<OptionKey>(key_);
auto value = JniUtils::JavaStringToString(value_, env);
(*options)[key] = value;
}

extern "C" JNIEXPORT void JNICALL
Java_com_test_ffmpegdemo_player_Options_setVoidPtr(JNIEnv *env, jobject thiz, jint key_, jlong ptr) {
std::shared_ptr<Options> options = JniUtils::CopyRefGet<Options>(env, thiz, kOptions);
auto key = static_cast<OptionKey>(key_);
auto value = reinterpret_cast<void *>(static_cast<intptr_t>(ptr));
(*options)[key] = value;
}


