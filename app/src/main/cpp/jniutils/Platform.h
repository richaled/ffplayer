#ifndef _JNI_UTILS_PLATFORM_H
#define _JNI_UTILS_PLATFORM_H
#include <string>

namespace JniUtils {

class Build
{
public:
    static std::string PRODUCT;
    static std::string DEVICE;
    static std::string BOARD;
    static std::string MANUFACTURER;
    static std::string BRAND;
    static std::string MODEL;
    static std::string HARDWARE;
    //static bool IS_EMULATOR;
    static std::string SERIAL;

    class VERSION
    {
    public:
        static std::string RELEASE;
        static int SDK_INT;
    };
};

bool IsEmulator();

}


#endif
