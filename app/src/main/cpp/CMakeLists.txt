cmake_minimum_required(VERSION 3.10.2)
project("ffmpegdemo")

add_subdirectory(ffmpeg)
add_subdirectory(glm)
add_subdirectory(jniutils)
add_subdirectory(player)

message("FFmpeglibs : ${FFmpeglibs}")
link_directories(${FFmpeglibs})

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


#target_link_libraries(overload -llianghao  -lpthread -lm -lstdc++)

target_link_libraries( # Specifies the target library.
        ${PROJECT_NAME}
        PRIVATE
        ${FFmpeg}
        glm
        JniUtil)

