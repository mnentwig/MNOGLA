#include "MNOGLA_util.h"

#include <stdexcept>
#include <string>
using std::string, std::runtime_error, std::to_string;
#include "../MNOGLA.h"

namespace MNOGLA {
void checkGlError(const char* op) {
    GLenum error = glGetError();
    if (!error)
        return;
    string e("GL error (" + string(op) + "):" + to_string(error));
    while ((error = glGetError()) != 0)
        e = e + ", " + to_string(error);
    logE("%s", e.c_str());
    throw runtime_error(e);
}

GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &pSource, nullptr);
    glCompileShader(shader);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        assert(infoLen);
        char* buf = (char*)malloc(infoLen);
        assert(buf);
        glGetShaderInfoLog(shader, infoLen, nullptr, buf);
        string e("Failed to load GL shader (" + string(shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") + "): " + string(buf));
        logE("%s", e.c_str());
        free(buf);
        throw std::runtime_error(e);
    }
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);

    GLuint program = glCreateProgram();
    if (!program) throw std::runtime_error("glCreateProgram() failed");
    glAttachShader(program, vertexShader);
    checkGlError("glAttachShader");
    glAttachShader(program, pixelShader);
    checkGlError("glAttachShader");
    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLint bufLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
        assert(bufLength);
        char* buf = (char*)malloc(bufLength);
        assert(buf);
        glGetProgramInfoLog(program, bufLength, nullptr, buf);
        string e("failed to link GL program: " + string(buf));
        logE("%s", e.c_str());
        free(buf);
        throw std::runtime_error(e);
    }
    return program;
}

namespace filledRect {
namespace prot {
static GLuint prog;
const GLuint argLoc_coord2d = 0;
const GLuint argLoc_rgb = 1;
}  // namespace prot
using namespace prot;
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
    glDrawArrays(GL_TRIANGLE_STRIP, /*first vertex*/0, /*vertex count*/4);
    checkGlError("glDrawArrays");
}
}  // namespace filledRect
void initUtil() {
    filledRect::init();
}
}  // namespace MNOGLA