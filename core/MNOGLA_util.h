#pragma once
#include "../MNOGLA.h"
#include "../core/MNOGLA_includeGl.h"

#define GLCHK(expr)                                         \
    {                                                       \
        (expr);                                             \
        ::MNOGLA::haltIfGlError(#expr, __FILE__, __LINE__); \
    }

namespace MNOGLA {
void util_initGlContext();
void util_deinit();

void haltIfGlError(const char* sourceExpr, const char* sourcefile, int sourceline);

void checkGlError(const char* op);
GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
GLint getAttribLoc(GLuint prog, const char* argName);
GLint getUniformLoc(GLuint prog, const char* argName);
}  // namespace MNOGLA
