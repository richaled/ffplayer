project(JniUtil)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type -Wextra")

file(GLOB_RECURSE SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(JniUtil ${SRCS} ${HEADERS})

set(JNIUTIL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "jniutils include dir")

find_library( # Sets the name of the path variable.
        log-lib
        log)

target_link_libraries( # Specifies the target library.
        JniUtil
        PRIVATE
        android
        EGL
        GLESv3
        ${log-lib})