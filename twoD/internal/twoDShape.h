#pragma once
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
    static GLint p0_scale;
    static GLint p0_offset;
    static void setOffsetScale(const ::glm::vec2& topLeft, const ::glm::vec2& bottomRight);

   private:
    // twoDShape is used by many classes. Initialize for the first, deinitialize for the last.
    static size_t initCount;
};
}  // namespace MNOGLA