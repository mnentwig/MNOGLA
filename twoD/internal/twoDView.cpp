#include "../twoDView.h"
#include "outlinedRect.h"
#include "filledRect.h"
#include "vectorText.h"
namespace MNOGLA {
twoDView_internal::twoDView_internal(float top, float left, float bottomOrHeight, float widthOrRight, bool absolutePt2)
    : topLeft(::glm::vec2(top, left)),
      bottomRight(absolutePt2 ? ::glm::vec2(bottomOrHeight, widthOrRight) : ::glm::vec2(top + bottomOrHeight, left + widthOrRight)) {}

twoDView::twoDView(float top, float left, float bottomOrHeight, float widthOrRight, bool absolutePt2)
    : twoDView_internal::twoDView_internal(top, left, bottomOrHeight, widthOrRight, absolutePt2) {}

void twoDView::filledRect(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb) {
    filledRect::draw(pt1, pt2, rgb, topLeft, bottomRight);
}
void twoDView::outlinedRect(const ::glm::vec2& pt1, const ::glm::vec2& pt2, float w, const ::glm::vec3& rgb) {
    outlinedRect::draw(pt1, pt2, w, rgb, topLeft, bottomRight);
}
void twoDView::vectorText(const ::glm::vec2& pt, const ::std::string& text, float height, const ::glm::vec3& rgb) {
    vectorText::draw(pt, text, height, rgb, topLeft, bottomRight);
}

}  // namespace MNOGLA