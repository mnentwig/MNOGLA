cmake_minimum_required(VERSION 3.4.1)
project(myProject)
# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")
add_library(MNOGLA_host SHARED
            host.cpp)

add_library(MNOGLA SHARED
        ../../../../../MNOGLA.cpp)

add_library(MNOGLA_userApp SHARED
        ../../../../../userApp/userApp.cpp)
target_include_directories(MNOGLA_userApp PRIVATE "include/glm")

target_link_libraries(MNOGLA_userApp
        MNOGLA
        EGL
        GLESv3)

# add lib dependencies
target_link_libraries(MNOGLA_host
        MNOGLA_userApp
        android
        log
)

