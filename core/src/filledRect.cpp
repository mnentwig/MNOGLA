#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "twoDShape.h"
namespace MNOGLA {
class filledRect : protected twoDShape {
   public:
    static void init();
    static void deinit();
    static void draw(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb, const ::glm::vec2& screen);
};

void filledRect::init() {
    twoDShape::init();
}

void filledRect::deinit() {
    twoDShape::deinit();
}

void filledRect::draw(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb, const ::glm::vec2& screen) {
    GLCHK(glUseProgram(p0));
    GLfloat vertices[4 * 2] = {pt1.x, pt1.y, pt1.x, pt2.y, pt2.x, pt1.y, pt2.x, pt2.y};
    GLCHK(glVertexAttribPointer(p0_coord2d, 2, GL_FLOAT, GL_FALSE, 0, vertices));
    GLCHK(glEnableVertexAttribArray(p0_coord2d));
    GLCHK(glVertexAttrib3f(p0_rgb, rgb.r, rgb.g, rgb.b));
    GLCHK(glDisableVertexAttribArray(p0_rgb));

    // === mapping ===
    GLCHK(glVertexAttrib2f(p0_scale, 2.0f / screen.x, -2.0f / screen.y));
    GLCHK(glDisableVertexAttribArray(p0_scale));
    GLCHK(glVertexAttrib2f(p0_offset, -1.0f, 1.0f));
    GLCHK(glDisableVertexAttribArray(p0_offset));

    GLCHK(glDrawArrays(GL_TRIANGLE_STRIP, /*first vertex*/ 0, /*vertex count*/ 4));
}

void draw_filledRect(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb, const ::glm::vec2& screen) {
    filledRect::draw(pt1, pt2, rgb, screen);
}
void init_filledRect() {
    filledRect::init();
}
void deinit_filledRect() {
    filledRect::deinit();
}

}  // namespace MNOGLA
