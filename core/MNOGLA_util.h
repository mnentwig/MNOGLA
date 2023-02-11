#pragma once
#include <glm/fwd.hpp>
#include <vector>

#include "../MNOGLA.h"
#include "../core/MNOGLA_includeGl.h"
using std::vector;

namespace MNOGLA {
typedef struct {
    float r;
    float g;
    float b;
} rgb_t;

typedef struct {
    float x;
    float y;
} xy_t;

void checkGlError(const char* op);
GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
GLint getArgLoc(GLuint prog, const char* argName);

void initUtil();
namespace filledRect {
void draw(const xy_t& xyA, const xy_t& xyB, const rgb_t& rgb);
}  // namespace filledRect
}  // namespace MNOGLA
namespace MNOGLA::text2d {
void draw(const char* text, const glm::vec2& pos, const glm::vec2& screenWH, float fontHeight, const glm::vec3& rgb);
}  // namespace MNOGLA::text2d
#include "../core/util_outlinedRect.h"