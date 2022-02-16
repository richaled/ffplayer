#include  <android/log.h>
#include <string>

#define  TAG    "xxxx"
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,__FILENAME__,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,__FILENAME__,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,__FILENAME__,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,__FILENAME__,__VA_ARGS__) // 定义LOGE类型
#define LOG(NAME,...)  __android_log_print(ANDROID_LOG_INFO,NAME,__VA_ARGS__) // 定义LOGI类型

#define FUN_BEGIN_TIME(FUN) {\
    long long t0 = GetSysCurrentTime();

#define FUN_END_TIME(FUN) \
    long long t1 = GetSysCurrentTime(); \
    LOG(__FILENAME__,"%s func cost time %ldms", FUN, (long)(t1-t0));}

#define BEGIN_TIME(FUN) {\
    LOGE("%s func start", FUN); \
    long long t0 = GetSysCurrentTime();

#define END_TIME(FUN) \
    long long t1 = GetSysCurrentTime(); \
    LOGE("%s func cost time %ldms", FUN, (long)(t1-t0));}

static long long GetSysCurrentTime()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    long long curTime = ((long long)(time.tv_sec))*1000+time.tv_usec/1000;
    return curTime;
}

#define GO_CHECK_GL_ERROR(...)   LOGE("CHECK_GL_ERROR %s glGetError = %d, line = %d, ",  __FUNCTION__, glGetError(), __LINE__)

#define DEBUG_LOGE(...) LOGE("DEBUG_LOGE %s line = %d",  __FUNCTION__, __LINE__)


#define METHOD LOGI("current func :%s",__func__);
#define METHODI LOG("%s",__func__);

#define THREAD_ID LOGI("current func : %s, thread id : %d",__func__, std::this_thread::get_id());

