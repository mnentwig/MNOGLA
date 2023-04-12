#pragma once
#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>

#include "../../core/MNOGLA_includeGl.h"
namespace MNOGLA {
class twoDShape {
   public:
    // create GL objects
    static void glInit();
    // delete GL objects. Note: First deinit affects ANY derived class!. Use only on application shutdown
    static void glDeinit();
    // default program for solid fill: program number
    static GLuint p0;
    // default program for solid fill: attribute location
    static GLint p0_coord2d;
    // default program for solid fill: uniform location
    static GLint p0_rgb;
    // default program for solid fill: uniform location
    static GLint p0_world2screen;
    static void setWorld2screen(const ::glm::mat3& world2screen);

   private:
    static bool canClean;
};
}  // namespace MNOGLA