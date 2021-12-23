#include <log/log.h>
#include "JvmOnLoad.h"
#include "JvmEnv.h"
#include "SymbolRegister.h"

namespace JniUtils {

#define MAX_AUTORUN_ITEM 1024

JvmOnLoadAutoRun::Closure **GetAutoRunTable()
{
    static JvmOnLoadAutoRun::Closure *_autoRunTable[MAX_AUTORUN_ITEM] = {nullptr};
    return _autoRunTable;
}

JvmOnLoadAutoRun::JvmOnLoadAutoRun(const Closure &closure) : mClosure(closure)
{
    JvmOnLoadAutoRun::Closure **table = GetAutoRunTable();
    for (int i = 0; i < MAX_AUTORUN_ITEM; ++i)
    {
        if (table[i] == nullptr)
        {
            table[i] = (JvmOnLoadAutoRun::Closure *)&mClosure;
            break;
        }
    }
}

static void RunAllAutoRunClosure()
{
    JvmOnLoadAutoRun::Closure **table = GetAutoRunTable();
    for (int i = 0; i < MAX_AUTORUN_ITEM; ++i)
    {
        if (table[i] == nullptr)
            break;
        (*table[i])();
    }
}


    static std::mutex gLoaderMutex;
    static jclass gClassLoaderClass = nullptr;
    static jobject gClassLoaderObject = nullptr;
    static jmethodID gFindClassMethodID = nullptr;



    jclass FindClass(JNIEnv *env, const std::string &className)
    {
        std::lock_guard<std::mutex> lock(gLoaderMutex);
        jclass clz = nullptr;
        if(gClassLoaderObject != nullptr)
        {
            std::vector<char> strBuf(className.size() + 1);
            memcpy(strBuf.data(), className.c_str(), className.size() + 1);
            std::replace(strBuf.begin(), strBuf.end(), '/', '.');
            jstring jClassName = env->NewStringUTF(strBuf.data());
            clz = reinterpret_cast<jclass>(env->CallObjectMethod(gClassLoaderObject, gFindClassMethodID, jClassName));
            env->DeleteLocalRef(jClassName);
            if(env->ExceptionCheck()) {
                env->ExceptionClear();
            }
        }
        else
        {
            LOGI("JNI_OnLoad not run? can't user class loader to load class: %s", className.c_str());
        }
        if(clz == nullptr)
        {
            clz = env->FindClass(className.c_str());
        }
        return clz;
    }

}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JniUtils::SetJvm(vm);
    JNIEnv *env = JniUtils::GetAttachedEnv();
    JniUtils::RegisterAllClass(env);
    JniUtils::RunAllAutoRunClosure();    

    return JNI_VERSION_1_6;
}

