#pragma once
#include <glm/vec3.hpp>
#include <string>

#include "internal/twoDView_internal.h"
namespace MNOGLA {

// Purpose: Draw 2d primitives
class twoDView : protected twoDView_internal {
   public:
//    twoDView();
    void set(::glm::vec2 center, ::glm::vec2 wh, float rot_rad);

    void filledRect(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb);
    void outlinedRect(const ::glm::vec2& pt1, const ::glm::vec2& pt2, float w, const ::glm::vec3& rgb);
    void vectorText(const ::glm::vec2& pt, const std::string& text, float height, const ::glm::vec3& rgb);
};
}  // namespace MNOGLA