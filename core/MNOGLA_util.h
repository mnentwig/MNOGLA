#pragma once
#include "../MNOGLA.h"
#include "../core/MNOGLA_includeGl.h"

#if true
#define GLCHK(expr)                                         \
    {                                                       \
        (expr);                                             \
        ::MNOGLA::haltIfGlError(#expr, __FILE__, __LINE__); \
    }
#else
// DEBUG: trace all checked GL calls e.g. to localize segfaults
#define GLCHK(expr)                                             \
    {   ::MNOGLA::logI("%s %s(%d)", #expr, __FILE__, __LINE__); \
        (expr);                                                 \
        ::MNOGLA::haltIfGlError(#expr, __FILE__, __LINE__);     \
    }
#endif
namespace MNOGLA {
void util_init();
void util_deinit();

void haltIfGlError(const char* sourceExpr, const char* sourcefile, int sourceline);

void checkGlError(const char* op);
GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
GLint getAttribLoc(GLuint prog, const char* argName);
GLint getUniformLoc(GLuint prog, const char* argName);
}  // namespace MNOGLA
