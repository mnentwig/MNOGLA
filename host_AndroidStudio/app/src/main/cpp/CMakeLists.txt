cmake_minimum_required(VERSION 3.4.1)
project(myProject)

# === build switches ===
set(MNOGLA_HAS_FREETYPE true) # enable optional freetype support 
# add_compile_definitions(MNOGLA_HASAUDIO)


# location of CMakeLists.txt in Android studio build
set(APP_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
# undo host_AndroidStudio/app/src/main/cpp => 5 levels up
set(MNOGLA_ROOT_DIR "${APP_ROOT_DIR}/../../../../..")
# host_AndroidStudio/app/src/main/assets
set(MNOGLA_ASSET_DIR "${APP_ROOT_DIR}/../assets")
# application-specific source code
set(USERAPP_SRC_DIR "${MNOGLA_ROOT_DIR}/userApp2")


# === GLM include ===
include_directories(SYSTEM PRIVATE "${MNOGLA_ROOT_DIR}/3rdPartyLicense/glm-master")

# compiler flags TBD adapt from MINGW
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

# === MNOGLA_freetype ===
# 3rd party library
if (MNOGLA_HAS_FREETYPE)
        set(FT_ROOT "${MNOGLA_ROOT_DIR}/3rdPartyLicense/freetype-VER-2-13-0/")
        add_compile_definitions(MNOGLA_HAS_FREETYPE)
        include_directories(SYSTEM PRIVATE ${FT_ROOT}/include)
        add_library(MNOGLA_freetype STATIC
                ${FT_ROOT}/src/base/ftsystem.c
                ${FT_ROOT}/src/base/ftinit.c
                ${FT_ROOT}/src/base/ftdebug.c
                ${FT_ROOT}/src/base/ftbase.c
                ${FT_ROOT}/src/base/ftbbox.c
                ${FT_ROOT}/src/base/ftglyph.c
                ${FT_ROOT}/src/base/ftbdf.c
                ${FT_ROOT}/src/base/ftbitmap.c
                ${FT_ROOT}/src/base/ftcid.c
                ${FT_ROOT}/src/base/ftfstype.c
                ${FT_ROOT}/src/base/ftgasp.c
                ${FT_ROOT}/src/base/ftgxval.c
                ${FT_ROOT}/src/base/ftmm.c
                ${FT_ROOT}/src/base/ftotval.c
                ${FT_ROOT}/src/base/ftpatent.c
                ${FT_ROOT}/src/base/ftpfr.c
                ${FT_ROOT}/src/base/ftstroke.c
                ${FT_ROOT}/src/base/ftsynth.c
                ${FT_ROOT}/src/base/fttype1.c
                ${FT_ROOT}/src/base/ftwinfnt.c
                ${FT_ROOT}/src/sfnt/sfnt.c
                ${FT_ROOT}/src/truetype/truetype.c
                ${FT_ROOT}/src/raster/raster.c
                ${FT_ROOT}/src/smooth/smooth.c
                ${FT_ROOT}/src/sdf/sdf.c
                ${FT_ROOT}/src/autofit/autofit.c
                ${FT_ROOT}/src/gzip/ftgzip.c
                ${FT_ROOT}/src/gxvalid/gxvalid.c
                ${FT_ROOT}/src/otvalid/otvalid.c
                ${FT_ROOT}/src/psaux/psaux.c
                ${FT_ROOT}/src/pshinter/pshinter.c
                ${FT_ROOT}/src/psnames/psnames.c        
)

        # required symbol for building freetype library
        target_compile_definitions(MNOGLA_freetype PUBLIC FT2_BUILD_LIBRARY)

        # copy expected default font
        file(COPY "${MNOGLA_ROOT_DIR}/3rdPartyLicense/Noto_Sans/NotoSans-Regular.ttf"
                DESTINATION ${MNOGLA_ASSET_DIR})
endif()

# === userApp ===
add_library(userApp SHARED
        ${USERAPP_SRC_DIR}/userApp.cpp)

target_link_libraries(userApp
        MNOGLA_core
        EGL
        GLESv3)

# === MNOGLA_core ===
add_library(MNOGLA_core SHARED
        ${MNOGLA_ROOT_DIR}/core/MNOGLA.cpp
        ${MNOGLA_ROOT_DIR}/core/MNOGLA_util.cpp)

target_link_libraries(MNOGLA_core
        EGL
        GLESv3)

if (MNOGLA_HAS_FREETYPE)
  target_link_libraries(MNOGLA_core
    MNOGLA_freetype
)
endif()

# === host ===
add_library(MNOGLA_host SHARED
        host.cpp)

target_link_libraries(MNOGLA_host
        userApp
        MNOGLA_core
        android
        log)
