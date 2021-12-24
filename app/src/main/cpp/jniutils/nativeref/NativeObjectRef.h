#pragma once
#include <memory>
#include <cstdint>
#include "../JniUtils.h"
//#include "jniutils/JniUtils.h"

namespace JniUtils {

struct NativeWrap;
std::shared_ptr<void> _CopyRefGet(JNIEnv *env, jobject obj, uint32_t typeId);
std::shared_ptr<void> _MoveRefGet(JNIEnv *env, jobject obj, uint32_t typeId);
NativeWrap *_NewRefWrap(const std::shared_ptr<void> &nativeObj, uint32_t typeId);

template<typename T>
NativeWrap *NewRefWrap(const std::shared_ptr<T> &nativeObj, uint32_t typeId) {
    return _NewRefWrap(nativeObj, typeId);
}

template<typename T>
jlong NewRefWrapJlong(const std::shared_ptr<T> &nativeObj, uint32_t typeId) {
    return static_cast<jlong>(reinterpret_cast<intptr_t>(NewRefWrap(nativeObj, typeId)));
}

template<typename T>
std::shared_ptr<T> CopyRefGet(JNIEnv *env, jobject obj, uint32_t typeId) {
    return std::static_pointer_cast<T>(_CopyRefGet(env, obj, typeId));
}

template<typename T>
std::shared_ptr<T> MoveRefGet(JNIEnv *env, jobject obj, uint32_t typeId) {
    return std::static_pointer_cast<T>(_MoveRefGet(env, obj, typeId));
}

// create a java object, the java class MUST:
// 1. extends NativeObjectRef
// 2. have a constructor, the param is: long wrapPtr
LocalJniObject NewJavaNativeObjectRef(JNIEnv *env, NativeWrap *wrap, const std::string &className);

// create a java object, the java class MUST:
// 1. extends NativeObjectRef
// 2. have a constructor, the param is: long wrapPtr
template<typename T>
LocalJniObject NewObjectWithNativeInstance(
        JNIEnv *env, const std::shared_ptr<T> &nativeObj, uint32_t typeId, const std::string &className) {
    return NewJavaNativeObjectRef(env, _NewRefWrap(nativeObj, typeId), className);
}
}

