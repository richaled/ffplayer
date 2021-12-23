#ifndef _JNIUTILS_ASSERT_H
#define _JNIUTILS_ASSERT_H
#include <cstdio>
#include <cstdlib>

#define Assert(a) \
    do { \
    if(!(a)) {\
        fprintf(stderr, "Assert(%s) failed.(%s#%d:%s)\n", \
            #a,  __FILE__, __LINE__, __FUNCTION__);\
        abort(); \
    }\
    } while(0) 

#endif