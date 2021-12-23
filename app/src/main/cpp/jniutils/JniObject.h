#ifndef _JNI_UTILS_JNI_OBJECT_H
#define _JNI_UTILS_JNI_OBJECT_H
#include <jni.h>
#include <memory>
#include "JvmEnv.h"

namespace JniUtils {

using GlobalJniObject = std::shared_ptr<_jobject>;

class LocalJniObject
{
public:
    LocalJniObject() = default;

    explicit LocalJniObject(jobject obj, JNIEnv *env) : mObj(obj), mEnv(env) {}

    ~LocalJniObject() { if(mObj) mEnv->DeleteLocalRef(mObj); }

    jobject get() { return mObj; }

    jobject leak() { jobject obj = mObj; mObj = nullptr; return obj; }

    GlobalJniObject toGlobalJniObject()
    {
        return GlobalJniObject(mEnv->NewGlobalRef(mObj), 
            [](_jobject *obj) { 
                AttachThreadLocalEnv()->DeleteGlobalRef(obj);
            });
    }


    LocalJniObject(LocalJniObject &&other)
    {
        mObj = other.mObj;
        mEnv = other.mEnv;
        other.mObj = nullptr;
        other.mEnv = nullptr;
    }

    LocalJniObject &operator=(LocalJniObject &&other)
    {
        mObj = other.mObj;
        mEnv = other.mEnv;
        other.mObj = nullptr;
        other.mEnv = nullptr;
        return *this;
    }

private:
    /* data */
    jobject mObj = nullptr;
    JNIEnv *mEnv = nullptr;

private:
    LocalJniObject(const LocalJniObject &) = delete;
    LocalJniObject &operator=(LocalJniObject &) = delete;
};

inline GlobalJniObject LocalRawObjectToGlobalRefObject(jobject localObj, JNIEnv *env)
{
    return GlobalJniObject(env->NewGlobalRef(localObj),
        [](_jobject *obj) {
            AttachThreadLocalEnv()->DeleteGlobalRef(obj);
        });
}

}
#endif
