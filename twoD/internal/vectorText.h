#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "twoDShape.h"

namespace MNOGLA {
class vectorText : protected twoDShape {
   public:
    static void glInit();
    static void glDeinit();
    static void draw(const ::glm::vec2& pt, const ::std::string& text, float height, const ::glm::vec3& rgb, const ::glm::mat3& world2screen);

   protected:
    static ::std::vector<::glm::vec2> text2lines(const ::glm::vec2& pt, const ::std::string& text, float height);
    static GLuint vertexBuf;
};
}  // namespace MNOGLA