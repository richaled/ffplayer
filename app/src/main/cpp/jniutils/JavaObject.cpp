#include "JavaObject.h"
#include <vector>
#include "Assert.h"
#include "JniObject.h"
#include "JavaTypes.h"
namespace JniUtils {

static void InitFields(JavaClassInfo *classInfo, JNIEnv *env)
{
    Assert(classInfo->jvmData != nullptr);
    Assert(classInfo->jvmData->clz != nullptr);
    JavaClassJvmData *jvmData = classInfo->jvmData;
    for(auto &item : classInfo->objFields)
    {
        jfieldID id = env->GetFieldID(jvmData->clz, item.name.c_str(), item.sig.c_str());
        if(id == nullptr)
        {
            fprintf(stderr, "field not found(name=%s, sig=%s)\n", item.name.c_str(), item.sig.c_str());
            continue;
        }
        if(jvmData->objFieldIds.find(item.name) != jvmData->objFieldIds.end())
        {
            fprintf(stderr, "already have one field: %s\n", item.name.c_str());
            Assert(false);
        }
        jvmData->objFieldIds[item.name] = id;
    }
    for(auto &item : classInfo->staticFields)
    {
        jfieldID id = env->GetStaticFieldID(jvmData->clz, item.name.c_str(), item.sig.c_str());
        if(id == nullptr)
        {
            fprintf(stderr, "static field not found(name=%s, sig=%s)\n", item.name.c_str(), item.sig.c_str());
            continue;
        }
        if(jvmData->staticFieldIds.find(item.name) != jvmData->staticFieldIds.end())
        {
            fprintf(stderr, "already have one static field: %s\n", item.name.c_str());
            Assert(false);
        }
        jvmData->staticFieldIds[item.name] = id;
    }
    for(auto &item : classInfo->objMethods)
    {
        jmethodID id = env->GetMethodID(jvmData->clz, item.name.c_str(), item.sig.c_str());
        if(id == nullptr)
        {
            fprintf(stderr, "method not found(name=%s, sig=%s)\n", item.name.c_str(), item.sig.c_str());
            continue;
        }
        if(jvmData->objMethodIds.find(item.aliasName) != jvmData->objMethodIds.end())
        {
            fprintf(stderr, "already have one method: %s\n", item.aliasName.c_str());
            Assert(false);
        }
        jvmData->objMethodIds[item.aliasName] = id;
    }
    for(auto &item : classInfo->staticMethods)
    {
        jmethodID id = env->GetStaticMethodID(jvmData->clz, item.name.c_str(), item.sig.c_str());
        if(id == nullptr)
        {
            fprintf(stderr, "static method not found(name=%s, sig=%s)\n", item.name.c_str(), item.sig.c_str());
            continue;
        }
        if(jvmData->staticMethodIds.find(item.aliasName) != jvmData->staticMethodIds.end())
        {
            fprintf(stderr, "already have one static method: %s\n", item.aliasName.c_str());
            Assert(false);
        }
        jvmData->staticMethodIds[item.aliasName] = id;
    }
    for(auto &item : classInfo->constructors)
    {
        jmethodID id = env->GetMethodID(jvmData->clz, item.name.c_str(), item.sig.c_str());
        if(id == nullptr)
        {
            fprintf(stderr, "constructor method not found(name=%s, sig=%s)\n", item.name.c_str(), item.sig.c_str());
            continue;
        }
        if(jvmData->constructorIds.find(item.aliasName) != jvmData->constructorIds.end())
        {
            fprintf(stderr, "already have one constructor with the same name: %s\n", item.aliasName.c_str());
            Assert(false);
        }
        jvmData->constructorIds[item.aliasName] = id;
    }
}

static void RegisterSymbolWithClassName(JavaClassInfo *classInfo, JNIEnv *env)
{
    Assert(classInfo->jvmData != nullptr);
    JavaClassJvmData *jvmData = classInfo->jvmData;
    LocalJniObject clz((jobject) (env->FindClass(classInfo->className.c_str())), env);
    Assert(clz.get() != nullptr);
    jvmData->clz = (jclass) env->NewGlobalRef(clz.get());
    InitFields(classInfo, env);
}

static void RegisterSymbolWithObject(JavaClassInfo *classInfo, jobject obj, JNIEnv *env)
{
    Assert(classInfo->jvmData != nullptr);
    JavaClassJvmData *jvmData = classInfo->jvmData;
    LocalJniObject clz((jobject) env->GetObjectClass(obj), env);
    Assert(clz.get() != nullptr);
    jvmData->clz = (jclass) env->NewGlobalRef(clz.get());
    InitFields(classInfo, env);
}

void OnceRegisterSymbolWithClassName(JavaClassInfo *classInfo, JNIEnv *env)
{
    std::call_once(classInfo->onceFlag, [=]() {
        RegisterSymbolWithClassName(classInfo, env);
    });
}

void OnceRegisterSymbolWithObject(JavaClassInfo *classInfo, jobject obj, JNIEnv *env)
{
    std::call_once(classInfo->onceFlag, [=]() {
        RegisterSymbolWithObject(classInfo, obj, env);
    });
}

JavaObject::JavaObject(JavaClassInfo *classInfo, jobject obj, JNIEnv *env)
    :
    mClassInfo(classInfo), mObj(obj), mEnv(env)
{
    if(obj != nullptr)
        OnceRegisterSymbolWithObject(classInfo, obj, env);
    else
        OnceRegisterSymbolWithClassName(classInfo, env);
}

void JavaObject::setBool(std::string fieldName, bool value)
{
    mEnv->SetBooleanField(mObj, getObjField(fieldName), (jboolean) value);
}

void JavaObject::setInt(std::string fieldName, int value)
{
    mEnv->SetIntField(mObj, getObjField(fieldName), (jint) value);
}

void JavaObject::setLong(std::string fieldName, int64_t value)
{
    mEnv->SetLongField(mObj, getObjField(fieldName), (jlong) value);
}

void JavaObject::setFloat(std::string fieldName, float value)
{
    mEnv->SetFloatField(mObj, getObjField(fieldName), (jfloat) value);
}

void JavaObject::setDouble(std::string fieldName, double value)
{
    mEnv->SetDoubleField(mObj, getObjField(fieldName), (jdouble) value);
}

void JavaObject::setString(std::string fieldName, std::string value)
{
    LocalJniObject str(StringToJavaString(value, mEnv));
    mEnv->SetObjectField(mObj, getObjField(fieldName), str.get());
}

void JavaObject::setObject(std::string fieldName, jobject obj)
{
    mEnv->SetObjectField(mObj, getObjField(fieldName), obj);
}

bool JavaObject::getBool(std::string fieldName)
{
    return mEnv->GetBooleanField(mObj, getObjField(fieldName));
}

int JavaObject::getInt(std::string fieldName)
{
    return mEnv->GetIntField(mObj, getObjField(fieldName));
}

int64_t JavaObject::getLong(std::string fieldName)
{
    return mEnv->GetLongField(mObj, getObjField(fieldName));
}

float JavaObject::getFloat(std::string fieldName)
{
    return mEnv->GetFloatField(mObj, getObjField(fieldName));
}

double JavaObject::getDouble(std::string fieldName)
{
    return mEnv->GetDoubleField(mObj, getObjField(fieldName));
}

std::string JavaObject::getString(std::string fieldName)
{
    LocalJniObject strObj(mEnv->GetObjectField(mObj, getObjField(fieldName)), mEnv);
    return JavaStringToString((jstring) strObj.get(), mEnv);
}

LocalJniObject JavaObject::getObject(std::string fieldName)
{
    LocalJniObject obj(mEnv->GetObjectField(mObj, getObjField(fieldName)), mEnv);
    return obj;
}

void JavaObject::callVoidMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    mEnv->CallVoidMethodV(mObj, mid, arg);
    va_end(arg);
}

bool JavaObject::callBoolMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    bool retv = mEnv->CallBooleanMethodV(mObj, mid, arg);
    va_end(arg);
    return retv;
}

int JavaObject::callIntMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    int retv = mEnv->CallIntMethodV(mObj, mid, arg);
    va_end(arg);
    return retv;
}

int64_t JavaObject::callLongMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    int64_t retv = mEnv->CallLongMethodV(mObj, mid, arg);
    va_end(arg);
    return retv;
}

float JavaObject::callFloatMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    float retv = mEnv->CallFloatMethodV(mObj, mid, arg);
    va_end(arg);
    return retv;
}

double JavaObject::callDoubleMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    double result = mEnv->CallDoubleMethodV(mObj, mid, arg);
    va_end(arg);
    return result;
}

std::string JavaObject::callStringMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject resultObj(mEnv->CallObjectMethodV(mObj, mid, arg), mEnv);
    va_end(arg);
    return JavaStringToString((jstring) resultObj.get(), mEnv);
}

LocalJniObject JavaObject::callObjectMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject resultObj(mEnv->CallObjectMethodV(mObj, mid, arg), mEnv);
    va_end(arg);
    return resultObj;
}

std::vector<bool> JavaObject::callBoolArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject resultObj(mEnv->CallObjectMethodV(mObj, mid, arg), mEnv);
    va_end(arg);
    return JavaBoolArrayToVector((jbooleanArray) resultObj.get(), mEnv);
}

std::vector<uint8_t> JavaObject::callByteArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject resultObj(mEnv->CallObjectMethodV(mObj, mid, arg), mEnv);
    va_end(arg);
    return JavaByteArrayToVector((jbyteArray) resultObj.get(), mEnv);
}

std::vector<int> JavaObject::callIntArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject resultObj(mEnv->CallObjectMethodV(mObj, mid, arg), mEnv);
    va_end(arg);
    return JavaIntArrayToVector((jintArray) resultObj.get(), mEnv);
}

std::vector<int64_t> JavaObject::callLongArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject resultObj(mEnv->CallObjectMethodV(mObj, mid, arg), mEnv);
    va_end(arg);
    return JavaLongArrayToVector((jlongArray) resultObj.get(), mEnv);
}

std::vector<float> JavaObject::callFloatArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject resultObj(mEnv->CallObjectMethodV(mObj, mid, arg), mEnv);
    va_end(arg);
    return JavaFloatArrayToVector((jfloatArray) resultObj.get(), mEnv);
}

std::vector<double> JavaObject::callDoubleArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject resultObj(mEnv->CallObjectMethodV(mObj, mid, arg), mEnv);
    va_end(arg);
    return JavaDoubleArrayToVector((jdoubleArray) resultObj.get(), mEnv);
}

std::vector<std::string> JavaObject::callStringArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject resultObj(mEnv->CallObjectMethodV(mObj, mid, arg), mEnv);
    va_end(arg);
    return JavaStringArrayToVector((jobjectArray)resultObj.get(), mEnv);
}

std::vector<LocalJniObject> JavaObject::callObjectArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject resultObj(mEnv->CallObjectMethodV(mObj, mid, arg), mEnv);
    va_end(arg);
    return JavaObjectArrayToVector((jobjectArray)resultObj.get(), mEnv);
}

void JavaObject::constructObject(std::string constructorName, LocalJniObject *object, ...)
{
    jmethodID mid = getConstructorMethodID(constructorName);
    va_list arg;
    va_start(arg, object);
    LocalJniObject resultObj(mEnv->NewObject(getClass(), mid, arg), mEnv);
    va_end(arg);
    mObj = resultObj.get();
    *object = std::move(resultObj);
}

jclass JavaObject::getClass()
{
    return mClassInfo->jvmData->clz;
}

jfieldID JavaObject::getObjField(std::string name)
{
    JavaClassJvmData *jvmData = mClassInfo->jvmData;
    auto itr = jvmData->objFieldIds.find(name);
    Assert(itr != jvmData->objFieldIds.end());
    return (*itr).second;
}

jfieldID JavaObject::getStaticField(std::string name)
{
    JavaClassJvmData *jvmData = mClassInfo->jvmData;
    auto itr = jvmData->staticFieldIds.find(name);
    Assert(itr != jvmData->staticFieldIds.end());
    return (*itr).second;
}

jmethodID JavaObject::getMethodID(std::string name)
{
    JavaClassJvmData *jvmData = mClassInfo->jvmData;
    auto itr = jvmData->objMethodIds.find(name);
    Assert(itr != jvmData->objMethodIds.end());
    return (*itr).second;
}

jmethodID JavaObject::getStaticMethodID(std::string name)
{
    JavaClassJvmData *jvmData = mClassInfo->jvmData;
    auto itr = jvmData->staticMethodIds.find(name);
    Assert(itr != jvmData->staticMethodIds.end());
    return (*itr).second;
}

jmethodID JavaObject::getConstructorMethodID(std::string name)
{
    JavaClassJvmData *jvmData = mClassInfo->jvmData;
    auto itr = jvmData->constructorIds.find(name);
    Assert(itr != jvmData->constructorIds.end());
    return (*itr).second;
}


jfieldID JavaObject::hasObjField(std::string name)
{
    JavaClassJvmData *jvmData = mClassInfo->jvmData;
    auto itr = jvmData->objFieldIds.find(name);
    if(itr != jvmData->objFieldIds.end())
        return (*itr).second;
    return nullptr;
}

jfieldID JavaObject::hasStaticField(std::string name)
{
    JavaClassJvmData *jvmData = mClassInfo->jvmData;
    auto itr = jvmData->staticFieldIds.find(name);
    if(itr != jvmData->staticFieldIds.end())
        return (*itr).second;
    return nullptr;
}

jmethodID JavaObject::hasMethodID(std::string name)
{
    JavaClassJvmData *jvmData = mClassInfo->jvmData;
    auto itr = jvmData->objMethodIds.find(name);
    if(itr != jvmData->objMethodIds.end())
        return (*itr).second;
    return nullptr;
}

jmethodID JavaObject::hasStaticMethodID(std::string name)
{
    JavaClassJvmData *jvmData = mClassInfo->jvmData;
    auto itr = jvmData->staticMethodIds.find(name);
    if(itr != jvmData->staticMethodIds.end())
        return (*itr).second;
    return nullptr;
}

jmethodID JavaObject::hasConstructorMethodID(std::string name)
{
    JavaClassJvmData *jvmData = mClassInfo->jvmData;
    auto itr = jvmData->constructorIds.find(name);
    if(itr != jvmData->constructorIds.end())
        return (*itr).second;
    return nullptr;
}

void JavaObject::callStaticVoidMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    mEnv->CallStaticVoidMethodV(getClass(), mid, arg);
    va_end(arg);
}

bool JavaObject::callStaticBoolMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    bool result = (bool)mEnv->CallStaticBooleanMethodV(getClass(), mid, arg);
    va_end(arg);
    return result;
}

int JavaObject::callStaticIntMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    int result = (int)mEnv->CallStaticIntMethodV(getClass(), mid, arg);
    va_end(arg);
    return result;
}

int64_t JavaObject::callStaticLongMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    int64_t result = mEnv->CallStaticLongMethodV(getClass(), mid, arg);
    va_end(arg);
    return result;
}

float JavaObject::callStaticFloatMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    float result = mEnv->CallStaticFloatMethodV(getClass(), mid, arg);
    va_end(arg);
    return result;
}

double JavaObject::callStaticDoubleMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    double result = mEnv->CallStaticDoubleMethodV(getClass(), mid, arg);
    va_end(arg);
    return result;
}

std::string JavaObject::callStaticStringMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject result(mEnv->CallStaticObjectMethodV(getClass(), mid, arg), mEnv);
    va_end(arg);
    return JavaStringToString((jstring)result.get(), mEnv);
}

LocalJniObject JavaObject::callStaticObjectMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject result(mEnv->CallStaticObjectMethodV(getClass(), mid, arg), mEnv);
    va_end(arg);
    return result;
}

std::vector<bool> JavaObject::callStaticBoolArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject result(mEnv->CallStaticObjectMethodV(getClass(), mid, arg), mEnv);
    va_end(arg);
    return JavaBoolArrayToVector((jbooleanArray)result.get(), mEnv);
}

std::vector<uint8_t> JavaObject::callStaticByteArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject result(mEnv->CallStaticObjectMethodV(getClass(), mid, arg), mEnv);
    va_end(arg);
    return JavaByteArrayToVector((jbyteArray)result.get(), mEnv);
}

std::vector<int> JavaObject::callStaticIntArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject result(mEnv->CallStaticObjectMethodV(getClass(), mid, arg), mEnv);
    va_end(arg);
    return JavaIntArrayToVector((jintArray)result.get(), mEnv);
}

std::vector<int64_t> JavaObject::callStaticLongArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject result(mEnv->CallStaticObjectMethodV(getClass(), mid, arg), mEnv);
    va_end(arg);
    return JavaLongArrayToVector((jlongArray)result.get(), mEnv);
}

std::vector<float> JavaObject::callStaticFloatArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject result(mEnv->CallStaticObjectMethodV(getClass(), mid, arg), mEnv);
    va_end(arg);
    return JavaFloatArrayToVector((jfloatArray)result.get(), mEnv);
}

std::vector<double> JavaObject::callStaticDoubleArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject result(mEnv->CallStaticObjectMethodV(getClass(), mid, arg), mEnv);
    va_end(arg);
    return JavaDoubleArrayToVector((jdoubleArray)result.get(), mEnv);
}

std::vector<std::string> JavaObject::callStaticStringArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject result(mEnv->CallStaticObjectMethodV(getClass(), mid, arg), mEnv);
    va_end(arg);
    return JavaStringArrayToVector((jobjectArray)result.get(), mEnv);
}

std::vector<LocalJniObject> JavaObject::callStaticObjectArrayMethod(std::string methodName, ...)
{
    jmethodID mid = getStaticMethodID(methodName);
    va_list arg;
    va_start(arg, methodName);
    LocalJniObject result(mEnv->CallStaticObjectMethodV(getClass(), mid, arg), mEnv);
    va_end(arg);
    return JavaObjectArrayToVector((jobjectArray)result.get(), mEnv);
}


}
