cmake_minimum_required(VERSION 3.4.1)
project(myProject)

#set(CMAKE_VERBOSE_MAKEFILE ON) # spam spam spam

# add_compile_definitions(MNOGLA_HASAUDIO)
if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "^MINGW")
#  add_compile_definitions(MNOGLA_HASWINMIDI)
endif()

set(MNOGLA_HAS_FREETYPE true) # enable optional freetype support 

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -pedantic -Weffc++ -fmax-errors=1")

# === GLM include ===
include_directories(SYSTEM PRIVATE 3rdPartyLicense/glm-master/include)

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
  )
  add_compile_definitions(MNOGLA_WINDOWS)
else()
  target_link_libraries(MNOGLA_core
      -static
    GL
    glfw
    stdc++
  )
endif()

if (MNOGLA_HAS_FREETYPE)
  target_link_libraries(MNOGLA_core
    -static
    MNOGLA_freetype
)
endif()

# === MNOGLA_freetype ===
# 3rd party library
if (MNOGLA_HAS_FREETYPE)
  set(FT "../3rdPartyLicense/freetype-VER-2-13-0/")
  add_compile_definitions(MNOGLA_HAS_FREETYPE)
  include_directories(SYSTEM PRIVATE ${FT}/include)
  add_library(MNOGLA_freetype STATIC
    ${FT}/src/base/ftsystem.c
    ${FT}/src/base/ftinit.c
    ${FT}/src/base/ftdebug.c
    ${FT}/src/base/ftbase.c
    ${FT}/src/base/ftbbox.c
    ${FT}/src/base/ftglyph.c
    ${FT}/src/base/ftbdf.c
    ${FT}/src/base/ftbitmap.c
    ${FT}/src/base/ftcid.c
    ${FT}/src/base/ftfstype.c
    ${FT}/src/base/ftgasp.c
    ${FT}/src/base/ftgxval.c
    ${FT}/src/base/ftmm.c
    ${FT}/src/base/ftotval.c
    ${FT}/src/base/ftpatent.c
    ${FT}/src/base/ftpfr.c
    ${FT}/src/base/ftstroke.c
    ${FT}/src/base/ftsynth.c
    ${FT}/src/base/fttype1.c
    ${FT}/src/base/ftwinfnt.c
    ${FT}/src/sfnt/sfnt.c
    ${FT}/src/truetype/truetype.c
    ${FT}/src/raster/raster.c
    ${FT}/src/smooth/smooth.c
    ${FT}/src/sdf/sdf.c
    ${FT}/src/autofit/autofit.c
    ${FT}/src/gzip/ftgzip.c
    ${FT}/src/gxvalid/gxvalid.c
    ${FT}/src/otvalid/otvalid.c
    ${FT}/src/psaux/psaux.c
    ${FT}/src/pshinter/pshinter.c
    ${FT}/src/psnames/psnames.c
 )

  target_include_directories(MNOGLA_freetype SYSTEM PRIVATE ../freetype-VER-2-13-0/include)
  target_compile_definitions(MNOGLA_freetype PUBLIC FT2_BUILD_LIBRARY)
  target_link_libraries(MNOGLA_freetype
    -static
    stdc++
  )

  file(COPY "../3rdPartyLicense/Noto_Sans/NotoSans-Regular.ttf"
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()


# === user application ===
# links to above MNOGLA_core
add_library(userApp STATIC
            ../userApp2/userApp.cpp
)
target_include_directories(userApp SYSTEM PRIVATE ../freetype-VER-2-13-0/include)

target_link_libraries(userApp
      -static
      MNOGLA_core
)

# === host ===
# links to above userApp. This needs to be recompiled on change, but is kept lightweight.
add_executable (host 
            host.cpp)

target_link_libraries(host
      -static
        userApp
)
