#ifndef _JNI_UTILS_JAVA_OBJECT_H
#define _JNI_UTILS_JAVA_OBJECT_H
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <jni.h>
#include <stdint.h>
#include "JniObject.h"

namespace JniUtils {


struct JavaClassJvmData
{
    jclass clz;
    std::map<std::string, jfieldID> objFieldIds;      // name -> jfieldID
    std::map<std::string, jfieldID> staticFieldIds;   // name -> jfieldID
    std::map<std::string, jmethodID> objMethodIds;    // alias name -> jmethodID
    std::map<std::string, jmethodID> staticMethodIds; // alias name -> jmethodID
    std::map<std::string, jmethodID> constructorIds;
};

struct FieldEntry
{
    std::string name;
    std::string sig;
};

// For a java class, multiple methods may have the same name, aliasName is set for make 
// every different, it should not be the same with others.
struct MethodEntry
{
    std::string name;
    std::string sig;
    std::string aliasName;

    MethodEntry(const std::string &name, const std::string &sig, const std::string &aliasName) :
        name(name), sig(sig), aliasName(aliasName)
    {}

    MethodEntry(const std::string &name, const std::string &sig) :
        name(name), sig(sig), aliasName(name)
    {}
};

struct JavaClassInfo
{
    std::string className;

    std::vector<FieldEntry> objFields;
    std::vector<FieldEntry> staticFields;
    std::vector<MethodEntry> objMethods;
    std::vector<MethodEntry> staticMethods;
    std::vector<MethodEntry> constructors;

    JavaClassJvmData *jvmData = nullptr;
    std::once_flag onceFlag;

public:
    JavaClassInfo(std::string className,
        const std::vector<FieldEntry> &objFds,
        const std::vector<FieldEntry> &staticFds,
        const std::vector<MethodEntry> &objMthds,
        const std::vector<MethodEntry> &staticMthds,
        const std::vector<MethodEntry> &constructorMthds,
        JavaClassJvmData *jvmData
    ) : className(className), objFields(objFds), staticFields(staticFds),
        objMethods(objMthds), staticMethods(staticMthds),
        constructors(constructorMthds),
        jvmData(jvmData)
    {}
};

void OnceRegisterSymbolWithClassName(JavaClassInfo *classInfo, JNIEnv *env);
void OnceRegisterSymbolWithObject(JavaClassInfo *classInfo, jobject obj, JNIEnv *env);

class JavaObject
{
public:
    JavaObject(JavaClassInfo *classInfo, jobject obj, JNIEnv *env);

    void constructObject(std::string constructorName, LocalJniObject *object, ...);

    void setBool(std::string fieldName, bool value);
    void setInt(std::string fieldName, int value);
    void setLong(std::string fieldName, int64_t value);
    void setFloat(std::string fieldName, float value);
    void setDouble(std::string fieldName, double value);
    void setString(std::string fieldName, std::string value);
    void setObject(std::string fieldName, jobject obj);

    bool getBool(std::string fieldName);
    int getInt(std::string fieldName);
    int64_t getLong(std::string fieldName);
    float getFloat(std::string fieldName);
    double getDouble(std::string fieldName);
    std::string getString(std::string fieldName);
    LocalJniObject getObject(std::string fieldName);
    
    void callVoidMethod(std::string methodName, ...);
    bool callBoolMethod(std::string methodName, ...);
    int  callIntMethod(std::string methodName, ...);
    int64_t callLongMethod(std::string methodName, ...);
    float callFloatMethod(std::string methodName, ...);
    double callDoubleMethod(std::string methodName, ...);
    std::string callStringMethod(std::string methodName, ...);
    LocalJniObject callObjectMethod(std::string methodName, ...);

    std::vector<bool> callBoolArrayMethod(std::string methodName, ...);
    std::vector<uint8_t> callByteArrayMethod(std::string methodName, ...);
    std::vector<int> callIntArrayMethod(std::string methodName, ...);
    std::vector<int64_t> callLongArrayMethod(std::string methodName, ...);
    std::vector<float> callFloatArrayMethod(std::string methodName, ...);
    std::vector<double> callDoubleArrayMethod(std::string methodName, ...);
    std::vector<std::string> callStringArrayMethod(std::string methodName, ...);
    std::vector<LocalJniObject> callObjectArrayMethod(std::string methodName, ...);

    void callStaticVoidMethod(std::string methodName, ...);
    bool callStaticBoolMethod(std::string methodName, ...);
    int  callStaticIntMethod(std::string methodName, ...);
    int64_t callStaticLongMethod(std::string methodName, ...);
    float callStaticFloatMethod(std::string methodName, ...);
    double callStaticDoubleMethod(std::string methodName, ...);
    std::string callStaticStringMethod(std::string methodName, ...);
    LocalJniObject callStaticObjectMethod(std::string methodName, ...);


    std::vector<bool> callStaticBoolArrayMethod(std::string methodName, ...);
    std::vector<uint8_t> callStaticByteArrayMethod(std::string methodName, ...);
    std::vector<int> callStaticIntArrayMethod(std::string method, ...);
    std::vector<int64_t> callStaticLongArrayMethod(std::string method, ...);
    std::vector<float> callStaticFloatArrayMethod(std::string method, ...);
    std::vector<double> callStaticDoubleArrayMethod(std::string method, ...);
    std::vector<std::string> callStaticStringArrayMethod(std::string method, ...);
    std::vector<LocalJniObject> callStaticObjectArrayMethod(std::string methondName, ...);


    jclass getClass();
    jfieldID getObjField(std::string name);
    jfieldID getStaticField(std::string name);
    jmethodID getMethodID(std::string name);
    jmethodID getStaticMethodID(std::string name);
    jmethodID getConstructorMethodID(std::string name);

    jfieldID hasObjField(std::string name);
    jfieldID hasStaticField(std::string name);
    jmethodID hasMethodID(std::string name);
    jmethodID hasStaticMethodID(std::string name);
    jmethodID hasConstructorMethodID(std::string name);


private:
    JavaClassInfo *mClassInfo = nullptr;
    jobject mObj = nullptr;
    JNIEnv *mEnv = nullptr;


    JavaObject(const JavaObject &) = delete;
    JavaObject &operator=(const JavaObject &) = delete;
};

}
#endif
