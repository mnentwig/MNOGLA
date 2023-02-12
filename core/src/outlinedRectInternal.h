#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../MNOGLA_includeGl.h"
namespace MNOGLA_internal {
class outlinedRectInternal {
   public:
   protected:
    static GLuint vertexBuf;
    static GLuint indexBuf;
    static GLuint prog;
    static GLint argLoc_coord2d;
    static GLint argLoc_rgb;
    static GLint argLoc_scale;
    static GLint argLoc_offset;
    static size_t nVertexIndices;
};
}  // namespace MNOGLA_internal
