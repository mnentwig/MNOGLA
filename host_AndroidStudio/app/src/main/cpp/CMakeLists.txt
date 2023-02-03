cmake_minimum_required(VERSION 3.4.1)
project(myProject)

#Android host supports audio
add_compile_definitions(MNOGLA_HASAUDIO)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")
add_library(MNOGLA_host SHARED
            host.cpp
            ../../../../../userApp/userApp.cpp
            ../../../../../MNOGLA.cpp
        )

# add lib dependencies
target_link_libraries(MNOGLA_host
        EGL
        GLESv3
        android
        log
        aaudio
)

