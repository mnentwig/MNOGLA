#include "../MNOGLA_util.h"

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

    prog = createProgram(vs, fs);

    // === argument locations ===
    argLoc_coord2d = getArgLoc(prog, "coord2d");
    argLoc_rgb = getArgLoc(prog, "rgb");
    argLoc_scale = getArgLoc(prog, "scale");
    argLoc_offset = getArgLoc(prog, "offset");

    // === buffers ===
    GLCHK(glGenBuffers(1, &vertexBuf));
    GLCHK(glGenBuffers(1, &indexBuf));

    // triangle strip point indices
    const GLushort vertexIndex[] = {0, 4, 1, 5, 3, 7, 2, 6, 0, 4};
    nVertexIndices = sizeof(vertexIndex) / sizeof(vertexIndex[0]);

    GLCHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf));
    GLCHK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, /*nBytes*/ sizeof(vertexIndex), /*ptr*/ &vertexIndex[0], GL_STATIC_DRAW));
}

/*static!*/ void outlinedRect::deinit() {
    GLCHK(glDeleteBuffers(1, &vertexBuf));
    GLCHK(glDeleteBuffers(1, &indexBuf));
}

/*static!*/ void outlinedRect::draw(const ::glm::vec2& a, const ::glm::vec2& b, float w, const ::glm::vec3& rgb, const ::glm::vec2& screen) {
    GLCHK(glUseProgram(prog));

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

    GLCHK(glDisable(GL_DEPTH_TEST));

    // == vertex locations ===
    GLCHK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuf));
    GLCHK(glVertexAttribPointer(argLoc_coord2d, /*xy*/ 2, GL_FLOAT, GL_FALSE, 0, NULL));
    GLCHK(glEnableVertexAttribArray(argLoc_coord2d));

    GLCHK(glBufferData(GL_ARRAY_BUFFER, /*nBytes*/ sizeof(vertexLocation), /*ptr*/ &vertexLocation[0], GL_STATIC_DRAW));
    GLCHK(glVertexAttribDivisor(argLoc_coord2d, 0));

    // === index list ===
    GLCHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf));

    // === color ===
    GLCHK(glVertexAttrib3f(argLoc_rgb, rgb.r, rgb.g, rgb.b));
    GLCHK(glDisableVertexAttribArray(argLoc_rgb));
    GLCHK(glVertexAttribDivisor(argLoc_rgb, 0));

    // === mapping ===
    GLCHK(glVertexAttrib2f(argLoc_scale, 2.0f / screen.x, -2.0f / screen.y));
    GLCHK(glDisableVertexAttribArray(argLoc_scale));
    GLCHK(glVertexAttrib2f(argLoc_offset, -1.0f, 1.0f));
    GLCHK(glDisableVertexAttribArray(argLoc_offset));

    // === draw ===
    GLCHK(glDrawElements(GL_TRIANGLE_STRIP, nVertexIndices, GL_UNSIGNED_SHORT, 0));

    // === clean up ===
    GLCHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    GLCHK(glDisableVertexAttribArray(argLoc_coord2d));
    GLCHK(glUseProgram(0));
}
}  // namespace MNOGLA
namespace MNOGLA_internal {
GLuint outlinedRectInternal::vertexBuf;
GLuint outlinedRectInternal::indexBuf;
GLuint outlinedRectInternal::prog;
GLint outlinedRectInternal::argLoc_coord2d;
GLint outlinedRectInternal::argLoc_rgb;
GLint outlinedRectInternal::argLoc_scale;
GLint outlinedRectInternal::argLoc_offset;
size_t outlinedRectInternal::nVertexIndices;
}  // namespace MNOGLA_internal