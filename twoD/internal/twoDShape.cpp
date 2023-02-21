#include "twoDShape.h"

#include <glm/vec2.hpp>

#include "../../core/MNOGLA_util.h"

namespace MNOGLA {
size_t twoDShape::initCount = 0;
void twoDShape::init() {
    if (!initCount) {
        const char* vShader =
            "#version 300 es\n"
            "in vec2 coord2d;\n"
            "uniform vec3 rgb;\n"
            "uniform vec2 scale;\n"
            "uniform vec2 offset;\n"
            "out vec3 rgbv;\n"
            "void main() {\n"
            "  gl_Position = vec4(coord2d.x * scale.x + offset.x, coord2d.y * scale.y + offset.y, 0.0, 1.0);\n"
            "  rgbv = rgb;\n"
            "}\n";

        const char* fragShader =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec3 rgbv;\n"
            "out vec4 fragmentColor;\n"
            "void main() {\n"
            "  fragmentColor = vec4(rgbv, 1.0f);\n"
            "}\n";

        p0 = createProgram(vShader, fragShader);
        p0_coord2d = getAttribLoc(p0, "coord2d");
        p0_rgb = getUniformLoc(p0, "rgb");
        p0_scale = getUniformLoc(p0, "scale");
        p0_offset = getUniformLoc(p0, "offset");
    }
    ++initCount;
}

void twoDShape::setOffsetScale(const glm::vec2& topLeft, const glm::vec2& bottomRight) {
    // Maxima CAS:
    // string(linsolve([sx*x1+ox = -1, sx*x2+ox = 1, sy*y1+oy = 1, sy * y2 + oy = -1], [sx, ox, sy, oy]));
    // "[sx = -2/(x1-x2),ox = (x2+x1)/(x1-x2),sy = 2/(y1-y2),oy = -(y2+y1)/(y1-y2)]"
    float x1 = topLeft.x;
    float x2 = bottomRight.x;
    float y1 = topLeft.y;
    float y2 = bottomRight.y;

    float sx = -2.0f / (x1 - x2);
    float ox = (x2 + x1) / (x1 - x2);
    float sy = 2.0f / (y1 - y2);
    float oy = -(y2 + y1) / (y1 - y2);

    GLCHK(glUniform2f(p0_scale, sx, sy));
    GLCHK(glUniform2f(p0_offset, ox, oy));
}

void twoDShape::deinit() {
    if (initCount == 0) throw ::std::runtime_error("twoDShape: more deinit() than init()");
    --initCount;
    if (initCount == 0) {
        GLCHK(glDeleteProgram(p0));
    }
}
GLuint twoDShape::p0;
GLint twoDShape::p0_coord2d;
GLint twoDShape::p0_rgb;
GLint twoDShape::p0_scale;
GLint twoDShape::p0_offset;
}  // namespace MNOGLA