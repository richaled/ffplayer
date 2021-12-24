cmake_minimum_required(VERSION 3.10.2)
project("ffmpegdemo")

add_subdirectory(ffmpeg)
add_subdirectory(glm)
add_subdirectory(jniutils)
add_subdirectory(player)

add_library(
        ${PROJECT_NAME}
        SHARED
        ${PLAYER_SRC}
        native-lib.cpp
        native_player.cpp)

include_directories(${FFmpeg_INCLUDE_DIRS})
include_directories(jniutils)
include_directories(${GLM_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${FFmpeg_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC jniutils)
target_include_directories(${PROJECT_NAME} PUBLIC glm)

target_link_libraries( # Specifies the target library.
        ${PROJECT_NAME}
        PRIVATE
        avfilter
        avcodec
        avformat
        avutil
        swresample
        swscale
        fdk-aac
        x264
        glm
        JniUtil)

