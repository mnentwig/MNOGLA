#pragma once
#include <glm/mat3x3.hpp>

namespace MNOGLA {
class twoDMatrix {
   public:
    static glm::mat3 translate(glm::vec2 t);
    static glm::mat3 scale(glm::vec2 s);
    static glm::mat3 rot(float phi_rad);
};
}  // namespace MNOGLA