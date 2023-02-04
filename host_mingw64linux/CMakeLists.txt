cmake_minimum_required(VERSION 3.4.1)
project(myProject)

#host does NOT support audio (yet)
# add_compile_definitions(MNOGLA_HASAUDIO)

# this CMAKE-file is specifically for a windows build
add_compile_definitions(MNOGLA_WINDOWS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -pedantic")

# === MNOGLA_core ===
# independent of both host and user app to avoid frequent recompilation.
add_library(MNOGLA_core STATIC
            ../core/MNOGLA.cpp
            ../core/MNOGLA_util.cpp
)

target_link_libraries(MNOGLA_core
        glew32 
        glfw3 
        opengl32 
        stdc++
)

# === user application ===
# links to above MNOGLA_core
add_library(userApp STATIC
            ../userApp/userApp.cpp
)

target_link_libraries(userApp
        MNOGLA_core
)

# === host ===
# links to above userApp. This needs to be recompiled on change, but is kept lightweight.
add_executable (host 
            host.cpp)

target_link_libraries(host
        userApp
)

