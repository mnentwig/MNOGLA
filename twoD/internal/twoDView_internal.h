#pragma once
#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>
namespace MNOGLA {
class twoDView_internal {
   protected:
    ::glm::mat3 world2screen;
    ::glm::mat3 screen2world;
};
}  // namespace MNOGLA