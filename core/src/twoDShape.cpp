#include "twoDShape.h"
namespace MNOGLA {
size_t twoDShape::initCount = 0;
void twoDShape::init() {
    if (!initCount) {
        const char* vShader =
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

        const char* fragShader =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec3 rgbv;\n"
            "out vec4 fragmentColor;\n"
            "void main() {\n"
            "  fragmentColor = vec4(rgbv, 1.0f);\n"
            "}\n";

        p0 = createProgram(vShader, fragShader);
        p0_coord2d = getArgLoc(p0, "coord2d");
        p0_rgb = getArgLoc(p0, "rgb");
        p0_scale = getArgLoc(p0, "scale");
        p0_offset = getArgLoc(p0, "offset");
    }
    ++initCount;
}
void twoDShape::deinit() {
    if (initCount == 0) throw ::std::runtime_error("twoDShape: more deinit() than init()");
    --initCount;
    if (initCount == 0)
        ;
}
GLuint twoDShape::p0;
GLint twoDShape::p0_coord2d;
GLint twoDShape::p0_rgb;
GLint twoDShape::p0_scale;
GLint twoDShape::p0_offset;
}  // namespace MNOGLA