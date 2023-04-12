#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "twoDShape.h"

namespace MNOGLA {
class filledRect : protected twoDShape {
   public:
    static void glInit();
    static void glDeinit();
    static void draw(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb, const ::glm::mat3& world2screen);
};
}  // namespace MNOGLA