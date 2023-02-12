#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../MNOGLA_includeGl.h"
namespace MNOGLA_internal {
class filledRectInternal {
   public:
   protected:
    static GLuint prog;
    static GLint argLoc_coord2d;
    static GLint argLoc_rgb;
    static GLint argLoc_scale;
    static GLint argLoc_offset;
};
}  // namespace MNOGLA_internal
