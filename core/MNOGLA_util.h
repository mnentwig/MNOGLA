#pragma once
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>  // move out of header
#include <stdexcept>
#include <string>
#include <vector>

#include "../MNOGLA.h"
#include "../core/MNOGLA_includeGl.h"

using std::vector;

#define GLCHK(expr)                                         \
    {                                                       \
        (expr);                                             \
        ::MNOGLA::haltIfGlError(#expr, __FILE__, __LINE__); \
    }

namespace MNOGLA {
void initUtil();
void deinitUtil();

void haltIfGlError(const char* sourceExpr, const char* sourcefile, int sourceline);

void checkGlError(const char* op);
GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
GLint getArgLoc(GLuint prog, const char* argName);

// move out of header
class twoDView_internal {
   public:
    twoDView_internal(float top, float left, float widthOrRight, float bottomOrHeight, bool absolutePt2);

   protected:
    ::glm::vec2 topLeft;
    ::glm::vec2 bottomRight;
};

class twoDView : protected twoDView_internal {
   public:
    twoDView(float top, float left, float widthOrRight, float bottomOrHeight, bool absolutePt2);

    void filledRect(const ::glm::vec2& pt1, const ::glm::vec2& pt2, const ::glm::vec3& rgb);
    void outlinedRect(const ::glm::vec2& pt1, const ::glm::vec2& pt2, float w, const ::glm::vec3& rgb);
    void vectorText(const ::glm::vec2& pt, const std::string& text, float height, const ::glm::vec3& rgb);
};
}  // namespace MNOGLA
