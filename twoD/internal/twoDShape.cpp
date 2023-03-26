#include "twoDShape.h"

#include <glm/vec2.hpp>

#include "../../core/MNOGLA_util.h"

namespace MNOGLA {
void twoDShape::init() {
    // Note: In case of GL context loss, init() will be called without deinit(). No need to glDelete() anything.
    const char* vShader =
        "#version 300 es\n"
        "in vec2 coord2d;\n"
        "uniform vec3 rgb;\n"
        "uniform mat3 world2screen;\n"
        "out vec3 rgbv;\n"
        "void main() {\n"
        "  gl_Position = vec4(world2screen*vec3(coord2d, 1.0f), 1.0);\n"
        "  rgbv = rgb;\n"
        "}\n";

    // RGB as uniform here?
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
    p0_world2screen = getUniformLoc(p0, "world2screen");
    canClean = true;
}

void twoDShape::setWorld2screen(const ::glm::mat3& world2screen) {
    GLCHK(glUniformMatrix3fv(p0_world2screen, /*num matrices*/ 1, /*transpose*/ false, &world2screen[0][0]));
}

void twoDShape::deinit() {
    // note: First deinit affects all derived classes. Use only on shutdown
    if (!canClean) return;
    canClean = false;
    GLCHK(glDeleteProgram(p0));
}

GLuint twoDShape::p0;
GLint twoDShape::p0_coord2d;
GLint twoDShape::p0_rgb;
GLint twoDShape::p0_world2screen;
bool twoDShape::canClean;
}  // namespace MNOGLA