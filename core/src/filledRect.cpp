#include "../MNOGLA_util.h"

namespace MNOGLA {
/*static!*/ void filledRect::init() {
    auto vShader =
        "#version 300 es\n"
        "layout (location = 0) in vec2 coord2d;\n"
        "layout (location = 1) in vec3 rgb;\n"
        "layout (location = 2) in vec2 scale;\n"
        "layout (location = 3) in vec2 offset;\n"
        "out vec3 rgbv;\n"
        "void main() {\n"
        "  gl_Position = vec4(coord2d.x * scale.x + offset.x, coord2d.y * scale.y + offset.y, 0.0, 1.0);\n"
        "  rgbv = rgb;\n"
        "}\n";

    auto fragShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec3 rgbv;\n"
        "out vec4 fragmentColor;\n"
        "void main() {\n"
        "  fragmentColor = vec4(rgbv, 1.0f);\n"
        "}\n";

    prog = createProgram(vShader, fragShader);
    argLoc_coord2d = getArgLoc(prog, "coord2d");
    argLoc_rgb = getArgLoc(prog, "rgb");
    argLoc_scale = getArgLoc(prog, "scale");
    argLoc_offset = getArgLoc(prog, "offset");
}

void filledRect::draw(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb, const ::glm::vec2& screen) {
    GLCHK(glUseProgram(prog));
    GLfloat vertices[4 * 2] = {pt1.x, pt1.y, pt1.x, pt2.y, pt2.x, pt1.y, pt2.x, pt2.y};
    GLCHK(glVertexAttribPointer(argLoc_coord2d, 2, GL_FLOAT, GL_FALSE, 0, vertices));
    GLCHK(glEnableVertexAttribArray(argLoc_coord2d));
    GLCHK(glVertexAttrib3f(argLoc_rgb, rgb.r, rgb.g, rgb.b));
    GLCHK(glDisableVertexAttribArray(argLoc_rgb));

    // === mapping ===
    GLCHK(glVertexAttrib2f(argLoc_scale, 2.0f / screen.x, -2.0f / screen.y));
    GLCHK(glDisableVertexAttribArray(argLoc_scale));
    GLCHK(glVertexAttrib2f(argLoc_offset, -1.0f, 1.0f));
    GLCHK(glDisableVertexAttribArray(argLoc_offset));

    GLCHK(glDrawArrays(GL_TRIANGLE_STRIP, /*first vertex*/ 0, /*vertex count*/ 4));
}
}  // namespace MNOGLA
namespace MNOGLA_internal {
GLuint filledRectInternal::prog;
GLint filledRectInternal::argLoc_coord2d;
GLint filledRectInternal::argLoc_rgb;
GLint filledRectInternal::argLoc_scale;
GLint filledRectInternal::argLoc_offset;
}  // namespace MNOGLA_internal
