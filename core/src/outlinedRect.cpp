#include "../MNOGLA_util.h"
#include "../util_outlinedRect.h"

namespace MNOGLA {
/*static!*/ void outlinedRect::init() {
    auto vs =
        "#version 300 es\n"
        "layout (location = 0) in vec2 coord2d;"
        "layout (location = 1) in vec3 rgb;"
        "layout (location = 2) in vec2 scale;"
        "layout (location = 3) in vec2 offset;"
        "out vec3 rgbv;"
        "void main(void) {"
        "     gl_Position = vec4(coord2d.x * scale.x + offset.x, coord2d.y * scale.y + offset.y, 0.0, 1.0);"
        "     rgbv = rgb;"
        "}";
    auto fs =
        "#version 300 es\n"
        "precision mediump float;"
        "in vec3 rgbv;"
        "out vec4 fragmentColor;"
        "void main(void) {"
        "   fragmentColor = vec4(rgbv, 1.0f);"
        "}";

    prog = MNOGLA::createProgram(vs, fs);

    argLoc_coord2d = MNOGLA::getArgLoc(prog, "coord2d");
    argLoc_rgb = MNOGLA::getArgLoc(prog, "rgb");
    argLoc_scale = MNOGLA::getArgLoc(prog, "scale");
    argLoc_offset = MNOGLA::getArgLoc(prog, "offset");
}

/*static!*/ void outlinedRect::draw(const ::glm::vec2& a, const ::glm::vec2& b, float w, const ::glm::vec3& rgb, const ::glm::vec2& screen) {
    glUseProgram(prog);
    MNOGLA::checkGlError("glUseProgram");

    // (a)0.........1...
    // ..(c)4.....5.....
    // .................
    // .....6..(d)7.....
    // ...2.........3(b)

    ::glm::vec2 c = a + glm::vec2(w, w);
    ::glm::vec2 d = b - glm::vec2(w, w);
    ::glm::vec2 vertexLocation[] = {
        ::glm::vec2(a.x, a.y),   // 0
        ::glm::vec2(b.x, a.y),   // 1
        ::glm::vec2(a.x, b.y),   // 2
        ::glm::vec2(b.x, b.y),   // 3
        ::glm::vec2(c.x, c.y),   // 4
        ::glm::vec2(d.x, c.y),   // 5
        ::glm::vec2(c.x, d.y),   // 6
        ::glm::vec2(d.x, d.y)};  // 7

    // triangle strip point indices
    const GLushort vertexIndex[] = {0, 4, 1, 5, 3, 7, 2, 6, 0, 4};
    const size_t nVertexIndices = sizeof(vertexIndex) / sizeof(vertexIndex[0]);

    // === buffers ===
    GLuint vertexBuf;
    GLuint indexBuf;
    glGenBuffers(1, &vertexBuf);
    MNOGLA::checkGlError("genBuf vertex");
    glGenBuffers(1, &indexBuf);
    MNOGLA::checkGlError("genBuf index");

    glDisable(GL_DEPTH_TEST);
    MNOGLA::checkGlError("depthtest");

    glUseProgram(prog);
    MNOGLA::checkGlError("glUseProgram");

    // == vertex locations ===
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
    MNOGLA::checkGlError("bindBuf vertex");

    glVertexAttribPointer(argLoc_coord2d, /*xy*/ 2, GL_FLOAT, GL_FALSE, 0, NULL);
    MNOGLA::checkGlError("glVertexAttribPointer");

    glEnableVertexAttribArray(argLoc_coord2d);
    MNOGLA::checkGlError("glEnableVertexAttribArray");

    glBufferData(GL_ARRAY_BUFFER, /*nBytes*/ sizeof(vertexLocation), /*ptr*/ &vertexLocation[0], GL_STATIC_DRAW);
    MNOGLA::checkGlError("bufData vertexLocation");

    glVertexAttribDivisor(argLoc_coord2d, 0);
    MNOGLA::checkGlError("vertexAttribDiv");

    // === index list ===
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);
    MNOGLA::checkGlError("bindBuf index");

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, /*nBytes*/ sizeof(vertexIndex), /*ptr*/ &vertexIndex[0], GL_STATIC_DRAW);
    MNOGLA::checkGlError("bufData line");

    // === color ===
    glVertexAttrib3f(argLoc_rgb, rgb.r, rgb.g, rgb.b);
    MNOGLA::checkGlError("glVertexAttrib3f");
    glDisableVertexAttribArray(argLoc_rgb);
    MNOGLA::checkGlError("disVertAttr");
    glVertexAttribDivisor(argLoc_rgb, 0);
    MNOGLA::checkGlError("vaDiv rgb");

    // === matrix ===
    glVertexAttrib2f(argLoc_scale, 2.0f / screen.x, 2.0f / screen.y);
    MNOGLA::checkGlError("va2f");
    glVertexAttrib2f(argLoc_offset, -1.0f, -1.0f);
    MNOGLA::checkGlError("va2f");

    // === draw ===
    glDrawElements(GL_TRIANGLE_STRIP, nVertexIndices, GL_UNSIGNED_SHORT, 0);
    MNOGLA::checkGlError("glDrawArrays");

    // === clean up ===
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    MNOGLA::checkGlError("bindBuf 0");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    MNOGLA::checkGlError("bindBuf 0");
    glDisableVertexAttribArray(argLoc_coord2d);
    MNOGLA::checkGlError("disVertAttr");
    glUseProgram(0);
    MNOGLA::checkGlError("useProg 0");
    glDeleteBuffers(1, &vertexBuf);
    MNOGLA::checkGlError("delBuf");
    glDeleteBuffers(1, &indexBuf);
    MNOGLA::checkGlError("delBuf");
}
}  // namespace MNOGLA
namespace MNOGLA_internal {
GLuint outlinedRectInternal::prog;
GLint outlinedRectInternal::argLoc_coord2d;
GLint outlinedRectInternal::argLoc_rgb;
GLint outlinedRectInternal::argLoc_scale;
GLint outlinedRectInternal::argLoc_offset;
}  // namespace MNOGLA_internal