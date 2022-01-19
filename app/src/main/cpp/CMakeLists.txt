cmake_minimum_required(VERSION 3.10.2)
project("ffmpegdemo")

add_subdirectory(ffmpeg)
add_subdirectory(glm)
add_subdirectory(LibeventBuild) #libevent库
add_subdirectory(jniutils) #jni工具类
add_subdirectory(player) #v1版本的播放器

link_directories(${FFmpeglibs})

#ev
file(GLOB_RECURSE EV_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/eventloop/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/eventloop/*.cc")

#player2
file(GLOB_RECURSE PLAYER2_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/player2/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/player2/*.cc")

#所有播放
set(ALL_PLAY_EDITOR_FILES
        ${EV_SRCS}
        ${PLAYER2_SRCS})


add_library(
        ${PROJECT_NAME}
        SHARED
        ${PLAYER_SRC}
        ${ALL_PLAY_EDITOR_FILES}
        native-lib.cpp
        native_player.cpp)

include_directories(${FFmpeg_INCLUDE_DIRS})
include_directories(jniutils)
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${LIBEVENT_INCLUDE})
target_include_directories(${PROJECT_NAME} PUBLIC ${FFmpeg_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC jniutils)
target_include_directories(${PROJECT_NAME} PUBLIC glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${LIBEVENT_INCLUDE})

#ffmpeg
set(FFmpeg
        avformat
        avcodec
        avfilter
        swresample
        swscale
        avutil
        fdk-aac
        x264
        )

target_link_libraries( # Specifies the target library.
        ${PROJECT_NAME}
        PRIVATE
        ${FFmpeg}
        glm
        JniUtil
        Libevent)

