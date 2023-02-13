#pragma once
#include <glm/fwd.hpp>
#include <stdexcept>
#include <string>
#include <vector>

#include "../MNOGLA.h"
#include "../core/MNOGLA_includeGl.h"

using std::vector;

#define GLCHK(expr)                                         \
    {                                                       \
        (expr);                                             \
        ::MNOGLA::haltIfGlError(#expr, __FILE__, __LINE__); \
    }

namespace MNOGLA {
void initUtil();
void deinitUtil();

void haltIfGlError(const char* sourceExpr, const char* sourcefile, int sourceline);

void checkGlError(const char* op);
GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
GLint getArgLoc(GLuint prog, const char* argName);

void draw_filledRect(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb, const ::glm::vec2& screen);
void draw_outlinedRect(const ::glm::vec2& pt1, const ::glm::vec2& pt2, float w, const ::glm::vec3& rgb, const ::glm::vec2& screen);
void draw_vectorText(const ::glm::vec2& pt, const std::string& text, float height, const ::glm::vec3& rgb, const ::glm::vec2& screen);
}  // namespace MNOGLA
