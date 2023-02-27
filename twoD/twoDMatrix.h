#pragma once
#include <glm/mat3x3.hpp>

namespace MNOGLA {
class twoDMatrix {
   public:
    static glm::mat3 translate(const glm::vec2& t);
    static glm::mat3 scale(const glm::vec2& s);
    static glm::mat3 scale(float f);

    static glm::mat3 rot(float phi_rad);
    static float matrixRotAngle_rad(const glm::mat3& m);
};
}  // namespace MNOGLA