#include "Platform.h"
#include "JavaObject.h"
#include "SymbolRegister.h"
#include "JvmOnLoad.h"
#include "JavaTypes.h"
#include <algorithm>

namespace JniUtils {

static JniUtils::JavaClassJvmData Build_JvmInfo;
static JniUtils::JavaClassInfo Build_JavaClass =
    {
        // class name
        "android/os/Build",
        // object fields
        {
        },
        // static fields
        {
            {"PRODUCT", "Ljava/lang/String;"},
            {"DEVICE", "Ljava/lang/String;"},
            {"BOARD", "Ljava/lang/String;"},
            {"MANUFACTURER", "Ljava/lang/String;"},
            {"BRAND", "Ljava/lang/String;"},
            {"MODEL", "Ljava/lang/String;"},
            {"HARDWARE", "Ljava/lang/String;"},
            //{"IS_EMULATOR", "Z"},
            {"SERIAL", "Ljava/lang/String;"},
        },
        // object methods
        {
        },
        // static methods
        {
        },
        // constructors
        {
        },
        &Build_JvmInfo
    };
static JniUtils::SymbolRegister _buildAutoRegister(&Build_JavaClass);


static JniUtils::JavaClassJvmData VERSION_JvmInfo;
static JniUtils::JavaClassInfo VERSION_JavaClass =
    {
        // class name
        "android/os/Build$VERSION",
        // object fields
        {
        },
        // static fields
        {
            {"RELEASE", "Ljava/lang/String;"},
            {"SDK_INT", "I"},
        },
        // object methods
        {
        },
        // static methods
        {
        },
        // constructors
        {
        },
        &VERSION_JvmInfo
    };
static JniUtils::SymbolRegister _versionAutoRegister(&VERSION_JavaClass);


std::string Build::PRODUCT;
std::string Build::DEVICE;
std::string Build::BOARD;
std::string Build::MANUFACTURER;
std::string Build::BRAND;
std::string Build::MODEL;
std::string Build::HARDWARE;
//bool Build::IS_EMULATOR = false;
std::string Build::SERIAL;

std::string Build::VERSION::RELEASE;
int Build::VERSION::SDK_INT = 0;

static JniUtils::JvmOnLoadAutoRun _autoSetPlatformInfo([]() {
    JNIEnv *env = AttachThreadLocalEnv();
    JavaObject buildCaller(&Build_JavaClass, nullptr, env);
    jclass  buildClass = buildCaller.getClass();
    jfieldID fid = nullptr;
    if((fid = buildCaller.getStaticField("PRODUCT")) != nullptr)
    {
        LocalJniObject obj(env->GetStaticObjectField(buildClass, fid), env);
        Build::PRODUCT = JavaStringToString((jstring)obj.get(), env);
    }
    if((fid = buildCaller.getStaticField("DEVICE")) != nullptr)
    {
        LocalJniObject obj(env->GetStaticObjectField(buildClass, fid), env);
        Build::DEVICE = JavaStringToString((jstring)obj.get(), env);
    }
    if((fid = buildCaller.getStaticField("BOARD")) != nullptr)
    {
        LocalJniObject obj(env->GetStaticObjectField(buildClass, fid), env);
        Build::BOARD = JavaStringToString((jstring)obj.get(), env);
    }
    if((fid = buildCaller.getStaticField("MANUFACTURER")) != nullptr)
    {
        LocalJniObject obj(env->GetStaticObjectField(buildClass, fid), env);
        Build::MANUFACTURER = JavaStringToString((jstring)obj.get(), env);
    }
    if((fid = buildCaller.getStaticField("BRAND")) != nullptr)
    {
        LocalJniObject obj(env->GetStaticObjectField(buildClass, fid), env);
        Build::BRAND = JavaStringToString((jstring)obj.get(), env);
    }
    if((fid = buildCaller.getStaticField("MODEL")) != nullptr)
    {
        LocalJniObject obj(env->GetStaticObjectField(buildClass, fid), env);
        Build::MODEL = JavaStringToString((jstring)obj.get(), env);
    }
    if((fid = buildCaller.getStaticField("HARDWARE")) != nullptr)
    {
        LocalJniObject obj(env->GetStaticObjectField(buildClass, fid), env);
        Build::MODEL = JavaStringToString((jstring)obj.get(), env);
    }
    if((fid = buildCaller.getStaticField("SERIAL")) != nullptr)
    {
        LocalJniObject obj(env->GetStaticObjectField(buildClass, fid), env);
        Build::SERIAL = JavaStringToString((jstring)obj.get(), env);
    }

    JavaObject versionCaller(&VERSION_JavaClass, nullptr, env);
    jclass  versionClass = versionCaller.getClass();
    if((fid = versionCaller.getStaticField("RELEASE")) != nullptr)
    {
        LocalJniObject obj(env->GetStaticObjectField(versionClass, fid), env);
        Build::VERSION::RELEASE = JavaStringToString((jstring)obj.get(), env);
    }
    if((fid = versionCaller.getStaticField("SDK_INT")) != nullptr)
    {
        Build::VERSION::SDK_INT = env->GetStaticIntField(versionClass, fid);
    }
});


static std::string StringToLowerCase(const std::string &str)
{
    std::string dst(str.c_str(), str.length());
    std::transform(str.begin(), str.end(), dst.begin(), ::tolower);
    return dst;
}

bool IsEmulator()
{
    using JniUtils::Build;
    if(StringToLowerCase(Build::PRODUCT).find("sdk") != std::string::npos ||
        StringToLowerCase(Build::MODEL).find("sdk") != std::string::npos ||
        StringToLowerCase(Build::MODEL).find("virtualbox") != std::string::npos)
        return true;
    (void)(0);
    return false;
}

}


