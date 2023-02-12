#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "src/outlinedRectInternal.h"
namespace MNOGLA {
class outlinedRect : protected MNOGLA_internal::outlinedRectInternal {
   public:
    static void init();
    static void deinit();
    static void draw(const ::glm::vec2& pt1, const ::glm::vec2& pt2, float w, const ::glm::vec3& rgb, const ::glm::vec2& screen);
};
}  // namespace MNOGLA
