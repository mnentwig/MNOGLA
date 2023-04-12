#include "filledRect.h"

#include "../../core/MNOGLA_util.h"
namespace MNOGLA {
void filledRect::glInit() {
    // Note: In case of GL context loss, init() will be called without deinit(). No need to glDelete() anything.
    twoDShape::glInit();
}

void filledRect::glDeinit() {
    twoDShape::glDeinit();
}

void filledRect::draw(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb, const ::glm::mat3& world2screen) {
    // === config ===
    GLCHK(glDisable(GL_DEPTH_TEST));
    GLCHK(glUseProgram(p0));

    // === vertices ===
    GLfloat vertices[4 * 2] = {pt1.x, pt1.y, pt1.x, pt2.y, pt2.x, pt1.y, pt2.x, pt2.y};
    GLCHK(glVertexAttribPointer(p0_coord2d, 2, GL_FLOAT, GL_FALSE, 0, vertices));
    GLCHK(glEnableVertexAttribArray(p0_coord2d));

    // === color ===
    GLCHK(glUniform3f(p0_rgb, rgb.r, rgb.g, rgb.b));

    // === mapping ===
    setWorld2screen(world2screen);

    // === draw ===
    GLCHK(glDrawArrays(GL_TRIANGLE_STRIP, /*first vertex*/ 0, /*vertex count*/ 4));
}

void glInit_filledRect() {
    filledRect::glInit();
}
void glDeinit_filledRect() {
    filledRect::glDeinit();
}

}  // namespace MNOGLA
