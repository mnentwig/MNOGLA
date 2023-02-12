#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "src/filledRectInternal.h"
namespace MNOGLA {
class filledRect : protected MNOGLA_internal::filledRectInternal {
   public:
    static void init();
    static void deinit();
    static void draw(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb, const ::glm::vec2& screen);
};
}  // namespace MNOGLA
