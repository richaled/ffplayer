#include "NativeObjectRef.h"
#include <mutex>
#include <map>
#include <log/log.h>

namespace JniUtils {

static JniUtils::JavaClassJvmData NativeObjectRef_JvmInfo;
static JniUtils::JavaClassInfo NativeObjectRef_ClassInfo =
    {
        // class name
        "com/test/ffmpegdemo/NativeObjectRef",
        // object fields
        {
            {"mWrapPtr", "J" },
        },
        // static fields
        {
        },
        // object methods
        {
            { "getWrapPtr", "()J" },
            { "moveGetWrapPtr", "()J" },
        },
        // static methods
        {
        },
        // constructors
        {
        },
        &NativeObjectRef_JvmInfo
    };
static JniUtils::SymbolRegister _autoRegister(&NativeObjectRef_ClassInfo);

struct NativeWrap {
    uint32_t typeId;
    std::shared_ptr<void> obj;
};

static std::shared_ptr<void> RefGet(JNIEnv *env, jobject obj, uint32_t typeId, bool move) {
    if(obj == nullptr){
        LOGE("native object ref is null, %u", typeId);
    }
    JavaObject caller(&NativeObjectRef_ClassInfo, obj, env);
    NativeWrap *wrap = reinterpret_cast<NativeWrap *>(
                static_cast<intptr_t>(caller.callLongMethod(move ? "moveGetWrapPtr" : "getWrapPtr")));
    if(wrap == nullptr) {
        return nullptr;
    }
    if(wrap->typeId != typeId){
        LOGE("bad native object ref, type not match! expect: %u, current: %u",
             typeId, wrap->typeId);
    }
    auto ref = wrap->obj;
    if(move) {
        delete wrap;
    }
    return ref;
}

std::shared_ptr<void> _CopyRefGet(JNIEnv *env, jobject obj, uint32_t typeId) {
    return RefGet(env, obj, typeId, false);
}

std::shared_ptr<void> _MoveRefGet(JNIEnv *env, jobject obj, uint32_t typeId) {
    return RefGet(env, obj, typeId, true);
}

NativeWrap *_NewRefWrap(const std::shared_ptr<void> &nativeObj, uint32_t typeId) {
    NativeWrap *wrap = new NativeWrap();
    wrap->typeId = typeId;
    wrap->obj = nativeObj;
    return wrap;
}

std::mutex gCacheMutex;
struct SymbolItem {
    jclass clz;
    jmethodID constructorMid;
};
std::map<std::string, SymbolItem> gCacheSymbols;

LocalJniObject NewJavaNativeObjectRef(JNIEnv *env, NativeWrap *wrap, const std::string &className) {
    std::lock_guard<std::mutex> lock(gCacheMutex);
    if(gCacheSymbols.count(className) <= 0) {
        jclass _clz = FindClass(env, className);
        if(_clz != nullptr){
            LOGE("NativeObjectRef: can't find class: %s", className.c_str());
        }
        jclass clz = reinterpret_cast<jclass>(env->NewGlobalRef(_clz));
        env->DeleteLocalRef(_clz);
        jmethodID constructorMid = env->GetMethodID(clz, "<init>", "(J)V");
        if(constructorMid != nullptr){
            LOGE("NativeObjectRef: no required constructor for class: %s", className.c_str());
        }
        gCacheSymbols.insert({className, SymbolItem{clz, constructorMid}});
    }

    SymbolItem symbolItem = gCacheSymbols.at(className);
    jobject _obj = env->NewObject(symbolItem.clz, symbolItem.constructorMid, static_cast<jlong>(reinterpret_cast<intptr_t>(wrap)));
    return LocalJniObject(_obj, env);
}

}

using namespace JniUtils;

extern "C" JNIEXPORT void JNICALL
Java_com_example_opengllearn_base_NativeObjectRef_nativeFree(JNIEnv *env, jobject instance) {
    JavaObject caller(&NativeObjectRef_ClassInfo, instance, env);
    NativeWrap *wrap = reinterpret_cast<NativeWrap *>(static_cast<intptr_t>(caller.getLong("mWrapPtr")));
    caller.setLong("mWrapPtr", 0);
    delete wrap;
}


extern "C" JNIEXPORT jboolean JNICALL
        Java_com_example_opengllearn_base_NativeObjectRef_nativeHasSameNativeObject(JNIEnv *env, jobject instance, jobject ref) {
    if(ref == nullptr || instance == nullptr) {
        return JNI_FALSE;
    }
    JavaObject caller(&NativeObjectRef_ClassInfo, instance, env);
    NativeWrap *wrap = reinterpret_cast<NativeWrap *>(static_cast<intptr_t>(caller.getLong("mWrapPtr")));
    JavaObject refCaller(&NativeObjectRef_ClassInfo, ref, env);
    NativeWrap *refWrap = reinterpret_cast<NativeWrap *>(static_cast<intptr_t>(refCaller.getLong("mWrapPtr")));
    return wrap->obj.get() == refWrap->obj.get() ? JNI_TRUE : JNI_FALSE;
}