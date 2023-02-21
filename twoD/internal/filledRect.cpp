#include "filledRect.h"

#include "../../core/MNOGLA_util.h"
namespace MNOGLA {
void filledRect::init() {
    twoDShape::init();
}

void filledRect::deinit() {
    twoDShape::deinit();
}

void filledRect::draw(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb, const ::glm::vec2& screenTopLeft, const ::glm::vec2& screenBottomRight) {
    GLCHK(glUseProgram(p0));
    GLfloat vertices[4 * 2] = {pt1.x, pt1.y, pt1.x, pt2.y, pt2.x, pt1.y, pt2.x, pt2.y};
    GLCHK(glVertexAttribPointer(p0_coord2d, 2, GL_FLOAT, GL_FALSE, 0, vertices));
    GLCHK(glEnableVertexAttribArray(p0_coord2d));
    GLCHK(glUniform3f(p0_rgb, rgb.r, rgb.g, rgb.b));

    // === mapping ===
    setOffsetScale(screenTopLeft, screenBottomRight);
    GLCHK(glDrawArrays(GL_TRIANGLE_STRIP, /*first vertex*/ 0, /*vertex count*/ 4));
}

void init_filledRect() {
    filledRect::init();
}
void deinit_filledRect() {
    filledRect::deinit();
}

}  // namespace MNOGLA
