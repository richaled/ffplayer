#include "JvmEnv.h"
#include <stdlib.h>
#include <cstdio>
#include "Assert.h"

namespace JniUtils {

static JavaVM *gJvm;

void SetJvm(JavaVM *vm)
{
    gJvm = vm;
}

JavaVM *GetJvm()
{
    return gJvm;
}

JNIEnv *GetAttachedEnv()
{
    JNIEnv *env = nullptr;
    gJvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    Assert(env != nullptr);
    return env;
}

#if defined(ANDROID) || defined(_ANDROID)
// see https://github.com/android-ndk/ndk/issues/216
// c++11 thread_local, clang stl: libc++ need NDK 14b

#include <pthread.h>

static pthread_key_t gKey;
static pthread_once_t gOnce = PTHREAD_ONCE_INIT;

static void _DetachJniWhenThreadExit(void *env)
{
    if(env != nullptr)
    {
        gJvm->DetachCurrentThread();
    }
}

static void _InitKey()
{
    pthread_key_create(&gKey, _DetachJniWhenThreadExit);
}

static void InitKeyOnce()
{
    pthread_once(&gOnce, _InitKey);
}

JNIEnv *AttachThreadLocalEnv()
{
    JNIEnv *env = nullptr;
    gJvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    if(env != nullptr)
        return env;

    InitKeyOnce();
    gJvm->AttachCurrentThread(&env, nullptr);
    pthread_setspecific(gKey, env);
    return env;
}


#else

class JNIEnvThreadInfo
{
public:
    JNIEnv *attach()
    {
        JNIEnv *env = nullptr;
        gJvm->AttachCurrentThread(&env, nullptr);
        mAttached = true;
        return env;
    }

    ~JNIEnvThreadInfo()
    {
        if(mAttached)
        {
            gJvm->DetachCurrentThread();
            mAttached = false;
        }
    }

private:
    bool mAttached = false;
};

JNIEnv *AttachThreadLocalEnv()
{
    static thread_local JNIEnvThreadInfo _autoDetachJni;
    JNIEnv *env = nullptr;
    gJvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    if(env != nullptr)
        return env;
    return _autoDetachJni.attach();
}

#endif

bool IsCurrentThreadAttached()
{
    JNIEnv *env = nullptr;
    gJvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    return env != nullptr;
}

void AttachCurrentThread()
{
    JNIEnv *env = nullptr;
    gJvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    Assert(env == nullptr);

    gJvm->AttachCurrentThread(&env, nullptr);
    Assert(env != nullptr);    
}

void DetachCurrentThread()
{
    gJvm->DetachCurrentThread();
}

} 
