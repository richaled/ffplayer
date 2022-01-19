project(LibeventBuild)

add_library(Libevent INTERFACE)

target_link_libraries(Libevent INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Android/${ANDROID_ABI}/lib/libevent.a)
target_include_directories(Libevent INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Android/${ANDROID_ABI}/include)

set(LIBEVENT_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/Android/${ANDROID_ABI}/include" CACHE INTERNAL "libevent include dir")


