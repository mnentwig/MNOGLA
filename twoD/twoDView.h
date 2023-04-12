#pragma once
#include <glm/vec3.hpp>
#include <string>

#include "internal/twoDView_internal.h"
namespace MNOGLA {

// Purpose: Draw 2d primitives
class twoDView : protected twoDView_internal {
   public:
    void set(::glm::vec2 center, ::glm::vec2 wh, float rot_rad);
    void setWorld2screen(const ::glm::mat3& world2screen);
    const glm::mat3& getWorld2screen() const;
    const glm::mat3& getScreen2world() const;

    void filledRect(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb);
    void outlinedRect(const ::glm::vec2& pt1, const ::glm::vec2& pt2, float w, const ::glm::vec3& rgb);
    void vectorText(const ::glm::vec2& pt, const std::string& text, float height, const ::glm::vec3& rgb);
    void textureText(const ::glm::vec2& pt, const std::string& text, float height, const ::glm::vec3& rgb);

    // convert normalized device coordinates to world
    glm::vec2 NDC2world(const glm::vec2& xy_NDC) const;
};
}  // namespace MNOGLA