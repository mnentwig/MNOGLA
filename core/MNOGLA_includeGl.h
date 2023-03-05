#pragma once
extern "C" {
#ifdef MNOGLA_WINDOWS
// Windows version uses glew as it comes with its own copy of relevant headers
// see also host side main.cpp (requires runtime initialization)

// disable DLL decorations - they aren't needed for linking against the dll but break static linkage
#define GLEW_STATIC 

#include <GL/glew.h>

// we get headers from glew - don't try to load via glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#else
#include <GLES3/gl31.h>
#endif
}

#include <cassert>
#include <stdexcept>