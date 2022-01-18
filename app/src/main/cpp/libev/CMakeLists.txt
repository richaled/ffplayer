cmake_minimum_required(VERSION 3.0)
project(libev) 

set(SRC_FILES
    ev.c
    event.c
)

configure_file(android/${ANDROID_ABI}/config.h config.h)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

add_library(ev STATIC ${SRC_FILES})

install(TARGETS ev ARCHIVE DESTINATION lib)
install(FILES  ev.h ev++.h event.h DESTINATION include)
