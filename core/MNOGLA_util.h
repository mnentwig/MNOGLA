#pragma once
#include "../MNOGLA.h"
#include "../core/MNOGLA_includeGl.h"

void checkGlError(const char* op);
GLuint loadShader(GLenum shaderType, const char* pSource, logFun_t logE) ;
GLuint createProgram(const char* pVertexSource, const char* pFragmentSource, logFun_t logE) ;
