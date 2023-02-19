cmake_minimum_required(VERSION 3.4.1)
project(myProject)

#host does NOT support audio (yet)
# add_compile_definitions(MNOGLA_HASAUDIO)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -pedantic -Weffc++")

# === MNOGLA_core ===
# independent of both host and user app to avoid frequent recompilation.
add_library(MNOGLA_core STATIC
            ../core/MNOGLA.cpp
            ../core/MNOGLA_util.cpp
)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "^MINGW")
  target_link_libraries(MNOGLA_core
    glew32 
    glfw3 
    opengl32 
    stdc++
    )
  add_compile_definitions(MNOGLA_WINDOWS)
else()
  target_link_libraries(MNOGLA_core
    GL
    glfw
    stdc++)
endif()

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

