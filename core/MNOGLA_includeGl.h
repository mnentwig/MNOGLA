#pragma once
#ifdef MNOGLA_WINDOWS
// Windows version uses glew as it comes with its own copy of relevant headers
// see also host side main.cpp (requires runtime initialization)
#include <GL/glew.h>
#else
#include <GLES3/gl31.h>
#endif
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cassert>
#include <stdexcept>