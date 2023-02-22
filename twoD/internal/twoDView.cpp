#include "../twoDView.h"

#include "../twoDMatrix.h"
#include "filledRect.h"
#include "outlinedRect.h"
#include "vectorText.h"
namespace MNOGLA {

glm::mat3 twoDView::get() {
    return this->world2screen;
}

void twoDView::set(const ::glm::mat3& world2screen) {
    this->world2screen = world2screen;
}

void twoDView::set(::glm::vec2 center, ::glm::vec2 wh, float rot_rad) {
    glm::vec2 scale(2.0f / wh.x, -2.0f / wh.y);
    world2screen = twoDMatrix::scale(scale) * twoDMatrix::translate(-center);
    return;
#if 0
    world2screen = glm::mat4(1.0f);
    // === shift centerPt to 0 ===
    world2screen[2][0] = -center.x;
    world2screen[2][1] = -center.y;
    // rot_rad = 30.0f * 3.1415f / 360.0f;
    if (rot_rad != 0.0) {
        world2screen[0][0] = cos(rot_rad);
        world2screen[0][1] = -sin(rot_rad);
        world2screen[1][0] = sin(rot_rad);
        world2screen[0][0] = cos(rot_rad);
    }

    // === scale from w, h to -1:1, invert y ===
    glm::vec2 scale(2.0f / wh.x, -2.0f / wh.y);
    world2screen[0][0] *= scale.x;
    world2screen[1][0] *= scale.x;
    world2screen[2][0] *= scale.x;
    world2screen[0][1] *= scale.y;
    world2screen[1][1] *= scale.y;
    world2screen[2][1] *= scale.y;

    // MNOGLA::logI("matrix");
    // for (size_t row = 0; row < 3; ++row)
    //     MNOGLA::logI("%1.3f\t%1.3f\t%1.3f", tmp[0][row], tmp[1][row], tmp[2][row]);
#endif
}

void twoDView::filledRect(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb) {
    filledRect::draw(pt1, pt2, rgb, world2screen);
}
void twoDView::outlinedRect(const ::glm::vec2& pt1, const ::glm::vec2& pt2, float w, const ::glm::vec3& rgb) {
    outlinedRect::draw(pt1, pt2, w, rgb, world2screen);
}
void twoDView::vectorText(const ::glm::vec2& pt, const ::std::string& text, float height, const ::glm::vec3& rgb) {
    vectorText::draw(pt, text, height, rgb, world2screen);
}

}  // namespace MNOGLA