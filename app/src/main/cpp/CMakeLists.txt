cmake_minimum_required(VERSION 3.10.2)
project("ffmpegdemo")

add_subdirectory(ffmpeg)
add_subdirectory(glm)
add_subdirectory(jniutils)

set(render_src render/VideoRender.cpp render/gl/GLUtils.cpp decoder/VideoDecoder.cpp)

add_library( # Sets the name of the library.
             ${PROJECT_NAME}
             # Sets the library as a shared library.
             SHARED
             # Provides a relative path to your source file(s).
            ${render_src}
             native-lib.cpp
        native_player.cpp)

include_directories(${FFmpeg_INCLUDE_DIRS})
include_directories(jniutils)
include_directories(glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${FFmpeg_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${JNIUTIL_INCLUDE_DIRS})
target_link_libraries( # Specifies the target library.
        ${PROJECT_NAME}
                        FFmpeg
                        glm
                        JniUtil)

