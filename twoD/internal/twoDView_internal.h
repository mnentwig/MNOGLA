#pragma once
#include <glm/vec2.hpp>
namespace MNOGLA {
class twoDView_internal {
   public:
    twoDView_internal(float top, float left, float widthOrRight, float bottomOrHeight, bool absolutePt2);

   protected:
    ::glm::vec2 topLeft;
    ::glm::vec2 bottomRight;
};
}