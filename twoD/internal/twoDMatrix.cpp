#include "../twoDMatrix.h"
namespace MNOGLA {
/*static*/ glm::mat3 twoDMatrix::translate(glm::vec2 t) {
    glm::mat3 r(1.0f);
    r[2][0] = t.x;
    r[2][1] = t.y;
    return r;
}

/*static*/ glm::mat3 twoDMatrix::scale(glm::vec2 s) {
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
}  // namespace MNOGLA