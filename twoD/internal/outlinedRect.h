#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "twoDShape.h"

namespace MNOGLA {
class outlinedRect : protected twoDShape {
   public:
    static void init();
    static void deinit();
    static void draw(const ::glm::vec2& a, const ::glm::vec2& b, float w, const ::glm::vec3& rgb, const ::glm::mat3& world2screen);

   protected:
    static GLuint vertexBuf;
    static GLuint indexBuf;
    static size_t nVertexIndices;
};

}  // namespace MNOGLA