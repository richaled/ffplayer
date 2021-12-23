#ifndef _JNI_UTILS_JAVA_TYPES_H
#define _JNI_UTILS_JAVA_TYPES_H
#include <jni.h>
#include <string>
#include <vector>
#include "JniObject.h"

namespace JniUtils {

LocalJniObject StringToJavaString(const std::string &str, JNIEnv *env);

LocalJniObject BoolsToJavaBoolArray(const bool *array, int count, JNIEnv *env);
LocalJniObject BytesToJavaByteArray(const void *buf, int size, JNIEnv *env);
LocalJniObject IntsToJavaIntArray(const int *array, int count, JNIEnv *env);
LocalJniObject Int64sToJavaLongArray(const int64_t *array, int count, JNIEnv *env);
LocalJniObject FloatsToJavaFloatArray(const float *array, int count, JNIEnv *env);
LocalJniObject DoublesToJavaDoubleArray(const double *array, int count, JNIEnv *env);
LocalJniObject StringVectorToJavaObjectArray(const std::vector<std::string> &array, JNIEnv *env);
LocalJniObject ObjectVectorToJavaObjectArray(const std::vector<LocalJniObject> &array, jclass objClass, JNIEnv *env);



std::string JavaStringToString(jstring obj, JNIEnv *env);

std::vector<bool> JavaBoolArrayToVector(jbooleanArray obj, JNIEnv *env);
std::vector<uint8_t> JavaByteArrayToVector(jbyteArray obj, JNIEnv *env);
std::vector<int> JavaIntArrayToVector(jintArray obj, JNIEnv *env);
std::vector<int64_t> JavaLongArrayToVector(jlongArray obj, JNIEnv *env);
std::vector<float> JavaFloatArrayToVector(jfloatArray obj, JNIEnv *env);
std::vector<double> JavaDoubleArrayToVector(jdoubleArray obj, JNIEnv *env);
std::vector<std::string> JavaStringArrayToVector(jobjectArray obj, JNIEnv *env);
std::vector<LocalJniObject> JavaObjectArrayToVector(jobjectArray obj, JNIEnv *env);

}


#endif
