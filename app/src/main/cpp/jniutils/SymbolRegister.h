#ifndef _JNIUTILS_SYMBOLREGISTER_H
#define _JNIUTILS_SYMBOLREGISTER_H
#include <jni.h>

namespace JniUtils {

struct JavaClassInfo;

class SymbolRegister
{
public:
    SymbolRegister(JavaClassInfo *javaClassInfo);

};

void RegisterAllClass(JNIEnv *env);

}
#endif
