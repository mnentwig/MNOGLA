#pragma once
#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>

#include "../../core/MNOGLA_includeGl.h"
namespace MNOGLA {
class twoDShape {
   public:
    static void init();
    static void deinit();
    static GLuint p0;
    static GLint p0_coord2d;
    static GLint p0_rgb;
    static GLint p0_world2screen;
    static void setWorld2screen(const ::glm::mat3& world2screen);

   private:
    // twoDShape is used by many classes. Initialize for the first, deinitialize for the last.
    static size_t initCount;
};
}  // namespace MNOGLA