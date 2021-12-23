#ifndef _JNI_UTILS_JVM_ONLOAD_H
#define _JNI_UTILS_JVM_ONLOAD_H

#include <functional>
#include <jni.h>
#include <mutex>
#include <algorithm>
#include <vector>

namespace JniUtils {

    class JvmOnLoadAutoRun {
    public:
        using Closure = std::function<void()>;

        JvmOnLoadAutoRun(const Closure &closure);

    private:
        Closure mClosure = nullptr;
    };

    jclass FindClass(JNIEnv *env, const std::string &className);

}

#endif

