#include "../twoDView.h"

#include "../twoDMatrix.h"
#include "filledRect.h"
#include "outlinedRect.h"
#include "vectorText.h"
#include "textureText.h"
namespace MNOGLA {

const glm::mat3& twoDView::getWorld2screen() const {
    return this->world2screen;
}

const glm::mat3& twoDView::getScreen2world() const {
    return this->screen2world;
}

glm::vec2 twoDView::NDC2world(const glm::vec2& xy_NDC) const {
    return this->screen2world * ::glm::vec3(xy_NDC, 1.0f);
}

void twoDView::setWorld2screen(const ::glm::mat3& world2screen) {
    this->world2screen = world2screen;
    this->screen2world = glm::inverse(world2screen);
}

void twoDView::set(::glm::vec2 center, ::glm::vec2 wh, float rot_rad) {
    glm::vec2 scale(2.0f / wh.x, -2.0f / wh.y);
    setWorld2screen(twoDMatrix::scale(scale) * twoDMatrix::rot(rot_rad) * twoDMatrix::translate(-center));
    return;

    // MNOGLA::logI("matrix");
    // for (size_t row = 0; row < 3; ++row)
    //     MNOGLA::logI("%1.3f\t%1.3f\t%1.3f", tmp[0][row], tmp[1][row], tmp[2][row]);
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

void twoDView::textureText(const ::glm::vec2& pt, const ::std::string& text, float height, const ::glm::vec3& rgb) {
    textureText::draw(pt, text, world2screen, rgb);
}

}  // namespace MNOGLA