cmake_minimum_required(VERSION 3.6)

project("FFmpeg")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(src-files empty.cpp)
message("src-file ${src-files}")

#add_library( # Sets the name of the library.
#        ${PROJECT_NAME}
#        SHARED
#        ${src-file}
#        )

add_library(avcodec
        SHARED
        IMPORTED GLOBAL)

set_target_properties(avcodec PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/lib/libavcodec.so
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/include
        )
add_library(avfilter
        SHARED
        IMPORTED GLOBAL)
set_target_properties(avfilter PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/lib/libavfilter.so
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/include
        )
add_library(avformat
        SHARED
        IMPORTED GLOBAL)
set_target_properties(avformat PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/lib/libavformat.so
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/include
        )
add_library(avutil
        SHARED
        IMPORTED GLOBAL)
set_target_properties(avutil PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/lib/libavutil.so
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/include
        )
add_library(swresample
        SHARED
        IMPORTED GLOBAL)
set_target_properties(swresample PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/lib/libswresample.so
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/include
        )
add_library(swscale
        SHARED
        IMPORTED GLOBAL)
set_target_properties(swscale PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/lib/libswscale.so
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/include
        )

add_library(fdk-aac
        SHARED
        IMPORTED GLOBAL)
set_target_properties(fdk-aac PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/lib/libfdk-aac.so
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/include
        )

add_library(x264
        STATIC
        IMPORTED GLOBAL)
set_target_properties(x264 PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/lib/libx264.a
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/include
        )

set(FFmpeg_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_ABI}/include" CACHE INTERNAL "ffmpeg include dir")

#[[
target_link_libraries(${PROJECT_NAME}
        avfilter
        avcodec
        avformat
        avutil
        swresample
        swscale
        fdk-aac
        x264
        )]]
