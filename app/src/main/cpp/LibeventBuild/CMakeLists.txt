project(LibeventBuild)

add_library(Libevent INTERFACE)

if (APPLE)
    if (IOS)
        target_link_libraries(Libevent INTERFACE
                ${CMAKE_CURRENT_SOURCE_DIR}/iOS/lib/libevent.a
                ${CMAKE_CURRENT_SOURCE_DIR}/iOS/lib/libevent_pthreads.a
                )
        target_include_directories(Libevent INTERFACE iOS/include)
    else ()
        target_link_libraries(Libevent INTERFACE
                ${CMAKE_CURRENT_SOURCE_DIR}/OSX/lib/libevent_core.a
                ${CMAKE_CURRENT_SOURCE_DIR}/OSX/lib/libevent_pthreads.a
                )
        target_include_directories(Libevent INTERFACE OSX/include)
    endif ()
elseif (WIN32)
        target_link_libraries(Libevent INTERFACE
                                      debug 
                                            ${CMAKE_CURRENT_SOURCE_DIR}/Win64/debug/lib/event_core.lib
                                      optimized 
                                            ${CMAKE_CURRENT_SOURCE_DIR}/Win64/release/lib/event_core.lib
                             )
        target_link_libraries(Libevent INTERFACE wsock32 ws2_32)
        target_include_directories(Libevent INTERFACE WIN64/include)
elseif (ANDROID)
    target_link_libraries(Libevent INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Android/${ANDROID_ABI}/lib/libevent.a)
    target_include_directories(Libevent INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Android/${ANDROID_ABI}/include)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if (Hi3559)
        target_link_libraries(Libevent INTERFACE
                ${CMAKE_CURRENT_SOURCE_DIR}/Hi3559/lib/libevent_core.a
                ${CMAKE_CURRENT_SOURCE_DIR}/Hi3559/lib/libevent_pthreads.a
                )
        target_include_directories(Libevent INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Hi3559/include)
    else ()
        target_link_libraries(Libevent INTERFACE
                ${CMAKE_CURRENT_SOURCE_DIR}/Linux/lib/libevent_core.a
                ${CMAKE_CURRENT_SOURCE_DIR}/Linux/lib/libevent_pthreads.a
                )
        target_include_directories(Libevent INTERFACE OSX/include)
    endif ()
endif ()

