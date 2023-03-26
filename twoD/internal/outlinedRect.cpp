#include "outlinedRect.h"

#include "../../core/MNOGLA_util.h"
namespace MNOGLA {
/*static!*/ void
outlinedRect::init() {
    // Note: In case of GL context loss, init() will be called without deinit(). No need to glDelete() anything.
    twoDShape::init();

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

/*static!*/ void outlinedRect::draw(const ::glm::vec2& a, const ::glm::vec2& b, float w, const ::glm::vec3& rgb, const ::glm::mat3& world2screen) {
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

    // === config ===
    GLCHK(glDisable(GL_DEPTH_TEST));
    GLCHK(glUseProgram(p0));

    // == vertex locations ===
    GLCHK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuf));
    GLCHK(glVertexAttribPointer(p0_coord2d, /*xy*/ 2, GL_FLOAT, GL_FALSE, 0, NULL));
    GLCHK(glEnableVertexAttribArray(p0_coord2d));

    GLCHK(glBufferData(GL_ARRAY_BUFFER, /*nBytes*/ sizeof(vertexLocation), /*ptr*/ &vertexLocation[0], GL_STATIC_DRAW));
    GLCHK(glVertexAttribDivisor(p0_coord2d, 0));  // remove

    // === index list ===
    GLCHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf));

    // === color ===
    GLCHK(glUniform3f(p0_rgb, rgb.r, rgb.g, rgb.b));

    // === mapping ===
    setWorld2screen(world2screen);

    // === draw ===
    GLCHK(glDrawElements(GL_TRIANGLE_STRIP, nVertexIndices, GL_UNSIGNED_SHORT, 0));

    // === clean up ===
    GLCHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    GLCHK(glDisableVertexAttribArray(p0_coord2d));
    GLCHK(glUseProgram(0));
}
GLuint outlinedRect::vertexBuf;
GLuint outlinedRect::indexBuf;
size_t outlinedRect::nVertexIndices;

void init_outlinedRect() {
    outlinedRect::init();
}
void deinit_outlinedRect() {
    outlinedRect::deinit();
}
}  // namespace MNOGLA