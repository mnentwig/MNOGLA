#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "twoDShape.h"

namespace MNOGLA {
class filledRect : protected twoDShape {
   public:
    static void init();
    static void deinit();
    static void draw(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb, const ::glm::vec2& screenTopLeft, const ::glm::vec2& screenBottomRight);
};
}  // namespace MNOGLA