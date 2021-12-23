#include "JavaTypes.h"
#include "JavaObject.h"
#include "SymbolRegister.h"

namespace JniUtils {

// register String class, as NewObjectArray for StringArray need the jclass of String
static JniUtils::JavaClassJvmData String_JvmInfo;
static JniUtils::JavaClassInfo String_ClassInfo =
    {
        // class name
        "java/lang/String",
        // object fields
        {
        },
        // static fields
        {
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
        &String_JvmInfo
    };
static JniUtils::SymbolRegister _autoRegister(&String_ClassInfo);

LocalJniObject StringToJavaString(const std::string &str, JNIEnv *env)
{
    return LocalJniObject(env->NewStringUTF(str.c_str()), env);
}


LocalJniObject BoolsToJavaBoolArray(const bool *array, int count, JNIEnv *env)
{
    std::vector<jboolean> boolArray((size_t)count);
    for(int i = 0; i < (int)boolArray.size(); ++i)
        boolArray[i] = (jboolean)array[i];
    LocalJniObject obj(env->NewBooleanArray(count), env);
    env->SetBooleanArrayRegion((jbooleanArray)obj.get(), 0, count, boolArray.data());
    return obj;
}

LocalJniObject BytesToJavaByteArray(const void *buf, int size, JNIEnv *env)
{
    LocalJniObject obj(env->NewByteArray(size), env);
    env->SetByteArrayRegion((jbyteArray)obj.get(), 0, size, (jbyte *)buf);
    return obj;
}

LocalJniObject IntsToJavaIntArray(const int *buf, int count, JNIEnv *env)
{
    LocalJniObject obj(env->NewIntArray(count), env);
    env->SetIntArrayRegion((jintArray)obj.get(), 0, count, (jint *)buf);
    return obj;
}

LocalJniObject Int64sToJavaLongArray(const int64_t *array, int count, JNIEnv *env)
{
    std::vector<jlong> longArray((size_t)count);
    for(int i = 0; i < (int)longArray.size(); ++i)
        longArray[i] = (jlong)array[i];
    LocalJniObject obj(env->NewLongArray(count), env);
    env->SetLongArrayRegion((jlongArray)obj.get(), 0, count, (jlong *)longArray.data());
    return obj;
}

LocalJniObject FloatsToJavaFloatArray(const float *array, int count, JNIEnv *env)
{
    LocalJniObject obj(env->NewFloatArray(count), env);
    env->SetFloatArrayRegion((jfloatArray)obj.get(), 0, count, (jfloat *)array);
    return obj;
}

LocalJniObject DoublesToJavaDoubleArray(const double *array, int count, JNIEnv *env)
{
    LocalJniObject obj(env->NewDoubleArray(count), env);
    env->SetDoubleArrayRegion((jdoubleArray)obj.get(), 0, count, (jdouble *)array);
    return obj;
}

LocalJniObject StringVectorToJavaObjectArray(const std::vector<std::string> &array, JNIEnv *env)
{
    LocalJniObject obj(env->NewObjectArray((jsize)array.size(), String_JvmInfo.clz, nullptr), env);
    for(int i = 0; i < (int)array.size(); ++i)
    {
        LocalJniObject strObj(StringToJavaString(array[i], env));
        env->SetObjectArrayElement((jobjectArray)obj.get(), i, strObj.get());
    }
    return obj;
}

LocalJniObject ObjectVectorToJavaObjectArray(const std::vector<LocalJniObject> &array, jclass objClass, JNIEnv *env)
{
    LocalJniObject obj(env->NewObjectArray((jsize)array.size(), objClass, nullptr), env);
    for(int i = 0; i < (int)array.size(); ++i)
    {
        env->SetObjectArrayElement((jobjectArray) obj.get(), i, ((LocalJniObject &) array[i]).get());
    }
    return obj;
}


std::string JavaStringToString(jstring obj, JNIEnv *env)
{
    if(obj == nullptr)
        return "";
    const char *buf = env->GetStringUTFChars(obj, nullptr);
    int len = env->GetStringUTFLength(obj);
    std::string str(buf, (size_t)len);
    env->ReleaseStringUTFChars(obj, buf);
    return str;
}

std::vector<bool> JavaBoolArrayToVector(jbooleanArray obj, JNIEnv *env)
{
    if(obj == nullptr)
        return std::vector<bool>();
    int len = env->GetArrayLength(obj);
    std::vector<jboolean > vec((size_t)len);
    env->GetBooleanArrayRegion(obj, 0, len, (jboolean *)vec.data());
    std::vector<bool> result(vec.size());
    for(int i = 0; i < (int)result.size(); ++i)
        result[i] = (bool)vec[i];
    return result;
}

std::vector<uint8_t> JavaByteArrayToVector(jbyteArray obj, JNIEnv *env)
{
    if(obj == nullptr)
        return std::vector<uint8_t>();
    int len = env->GetArrayLength(obj);
    std::vector<uint8_t> vec((size_t)len);
    env->GetByteArrayRegion(obj, 0, len, (jbyte *)vec.data());
    return vec;
}

std::vector<int> JavaIntArrayToVector(jintArray obj, JNIEnv *env)
{
    if(obj == nullptr)
        return std::vector<int>();
    int len = env->GetArrayLength(obj);
    std::vector<int> vec((size_t)len);
    env->GetIntArrayRegion(obj, 0, len, (jint *)vec.data());
    return vec;
}

std::vector<int64_t> JavaLongArrayToVector(jlongArray obj, JNIEnv *env)
{
    if(obj == nullptr)
        return std::vector<int64_t>();
    int len = env->GetArrayLength(obj);
    std::vector<jlong> longArray((size_t)len);
    env->GetLongArrayRegion(obj, 0, len, (jlong *)longArray.data());
    std::vector<int64_t> result(longArray.size());
    for(int i = 0; i < (int)result.size(); ++i)
        result[i] = longArray[i];
    return result;
}

std::vector<float> JavaFloatArrayToVector(jfloatArray obj, JNIEnv *env)
{
    if(obj == nullptr)
        return std::vector<float>();
    int len = env->GetArrayLength(obj);
    std::vector<float> vec((size_t)len);
    env->GetFloatArrayRegion(obj, 0, len, vec.data());
    return vec;
}

std::vector<double> JavaDoubleArrayToVector(jdoubleArray obj, JNIEnv *env)
{
    if(obj == nullptr)
        return std::vector<double>();
    int len = env->GetArrayLength(obj);
    std::vector<double> vec((size_t)len);
    env->GetDoubleArrayRegion(obj, 0, len, vec.data());
    return vec;
}

std::vector<std::string> JavaStringArrayToVector(jobjectArray array, JNIEnv *env)
{
    if(array == nullptr)
        return std::vector<std::string>();
    int len = env->GetArrayLength(array);
    std::vector<std::string> vec((size_t)len);
    for(int i = 0; i < len; ++i)
    {
        LocalJniObject element(env->GetObjectArrayElement(array, i), env);
        vec[i] = JavaStringToString((jstring)element.get(), env);
    }
    return vec;
}

std::vector<LocalJniObject> JavaObjectArrayToVector(jobjectArray array, JNIEnv *env)
{
    if(array == nullptr)
        return std::vector<LocalJniObject>();
    int len = env->GetArrayLength(array);
    std::vector<LocalJniObject> vec((size_t)len);
    for(int i = 0; i < len; ++i)
    {
        LocalJniObject element(env->GetObjectArrayElement(array, i), env);
        vec[i] = std::move(element);
    }
    return vec;
}



}