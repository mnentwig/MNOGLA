#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "twoDShape.h"

namespace MNOGLA {
class vectorText : protected twoDShape {
   public:
    static void init();
    static void deinit();
    static void draw(const ::glm::vec2& pt, const ::std::string& text, float height, const ::glm::vec3& rgb, const ::glm::vec2& screenTopLeft, const ::glm::vec2& screenBottomRight);

   protected:
    static ::std::vector<::glm::vec2> text2lines(const ::glm::vec2& pt, const ::std::string& text, float height);
    static GLuint vertexBuf;
};
}  // namespace MNOGLA