#pragma once
#include "../MNOGLA.h"
#include "../core/MNOGLA_includeGl.h"

namespace MNOGLA {
void checkGlError(const char* op);
GLuint loadShader(GLenum shaderType, const char* pSource);
GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
void initUtil();
namespace filledRect {
void init(logFun_t logE);
void drawXYXY(float x0, float y0, float x1, float y1);
}  // namespace filledRect
} // NS MNOGLA