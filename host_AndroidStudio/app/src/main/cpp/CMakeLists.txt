cmake_minimum_required(VERSION 3.4.1)
project(myProject)

include_directories(SYSTEM .)

#Android host supports audio
add_compile_definitions(MNOGLA_HASAUDIO)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

add_library(MNOGLA_core SHARED
        ../../../../../core/MNOGLA.cpp
        ../../../../../core/MNOGLA_util.cpp)

add_library(MNOGLA_host SHARED
        host.cpp)

add_library(userApp SHARED
        ../../../../../userApp/userApp.cpp)

target_link_libraries(userApp
        MNOGLA_core
        EGL
        GLESv3
        z)

target_link_libraries(MNOGLA_core
        EGL
        GLESv3)

target_link_libraries(MNOGLA_host
        userApp
        MNOGLA_core
        android
        log
        aaudio)
