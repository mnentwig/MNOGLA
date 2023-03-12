cmake_minimum_required(VERSION 3.4.1)
project(myProject)

#set(CMAKE_VERBOSE_MAKEFILE ON) # spam spam spam
add_compile_definitions(MNOGLA_HASAUDIO)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "^MINGW")
  add_compile_definitions(MNOGLA_HASWINMIDI)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -pedantic -Weffc++ -fmax-errors=1")

# === MNOGLA_core ===
# independent of both host and user app to avoid frequent recompilation.
add_library(MNOGLA_core STATIC
            ../core/MNOGLA.cpp
            ../core/MNOGLA_util.cpp
)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "^MINGW")
  target_link_libraries(MNOGLA_core
      -static
    glew32 
    glfw3 
      gdi32       # for glfw3
    opengl32 
    stdc++
    portaudio
      winmm       # for portaudio 
      ole32       # for portaudio 
      setupapi    # for portaudio 
    )
  add_compile_definitions(MNOGLA_WINDOWS)
else()
  target_link_libraries(MNOGLA_core
      -static
    GL
    glfw
    stdc++
    portaudio
)
endif()

# === user application ===
# links to above MNOGLA_core
add_library(userApp STATIC
            ../userApp/userApp.cpp
)

target_link_libraries(userApp
      -static
        MNOGLA_core
        z # for ods reader
)

# === host ===
# links to above userApp. This needs to be recompiled on change, but is kept lightweight.
add_executable (host 
            host.cpp)

target_link_libraries(host
      -static
        userApp
)

# copy resource file to same location as executable
configure_file(../userApp/synth/hello.ods hello.ods COPYONLY)
