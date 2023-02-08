#include "MNOGLA_includeGl.h"
#include "MNOGLA_util.h"
namespace MNOGLA{
namespace filledRect {
namespace details { 
static GLuint prog;
const GLuint argLoc_coord2d = 0;
const GLuint argLoc_rgb = 1;
}  // namespace details
using namespace details;
void init() {
    auto vShader =
        "#version 300 es\n"
        "layout (location = 0) in vec2 coord2d;\n"
        "layout (location = 1) in vec3 rgb;\n"
        "out vec3 rgbv;\n"
        "void main() {\n"
        "  gl_Position = vec4(coord2d, 0, 1.0f);\n"
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
}

void draw(const xy_t& xyA, const xy_t& xyB, const rgb_t& rgb) {
    glUseProgram(prog);
    checkGlError("glUseProgram");
    GLfloat vertices[4 * 2] = {xyA.x, xyA.y, xyA.x, xyB.y, xyB.x, xyA.y, xyB.x, xyB.y};
    glVertexAttribPointer(argLoc_coord2d, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(argLoc_coord2d);
    checkGlError("glEnableVertexAttribArray");
    glVertexAttrib3f(argLoc_rgb, rgb.r, rgb.g, rgb.b);
    checkGlError("glVertexAttrib3f");
    glDisableVertexAttribArray(argLoc_rgb);
    checkGlError("disVertAttr");

    glDrawArrays(GL_TRIANGLE_STRIP, /*first vertex*/ 0, /*vertex count*/ 4);
    checkGlError("glDrawArrays");
}
}  // namespace filledRect
} // namespace MNOGLA