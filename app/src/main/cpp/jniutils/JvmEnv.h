#ifndef _AR_JVM_H
#define _AR_JVM_H
#include <jni.h>
namespace JniUtils {


JavaVM *GetJvm();

JNIEnv *GetAttachedEnv();

JNIEnv *AttachThreadLocalEnv();

bool IsCurrentThreadAttached();

void AttachCurrentThread();

void DetachCurrentThread();

void SetJvm(JavaVM *vm);

}
#endif
