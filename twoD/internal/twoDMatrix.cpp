#include "../twoDMatrix.h"

#include <cmath>
namespace MNOGLA {
/*static*/ glm::mat3 twoDMatrix::translate(const glm::vec2& t) {
    glm::mat3 r(1.0f);
    r[2][0] = t.x;
    r[2][1] = t.y;
    return r;
}

/*static*/ glm::mat3 twoDMatrix::scale(const glm::vec2& s) {
    glm::mat3 r(1.0f);
    r[0][0] = s.x;
    r[1][1] = s.y;
    return r;
}

/*static*/ glm::mat3 twoDMatrix::scale(float f) {
    return twoDMatrix::scale(glm::vec2(f, f));
}

/*static*/ glm::mat3 twoDMatrix::rot(float phi_rad) {
    glm::mat3 r(1.0f);
    r[0][0] = cos(phi_rad);
    r[0][1] = -sin(phi_rad);
    r[1][0] = sin(phi_rad);
    r[1][1] = cos(phi_rad);
    return r;
}

/*static*/ float twoDMatrix::matrixRotAngle_rad(const glm::mat3& m) {
    // https://math.stackexchange.com/questions/301319/derive-a-rotation-from-a-2d-rotation-matrix
    return std::atan2(m[1][0], m[0][0]);
}
}  // namespace MNOGLA