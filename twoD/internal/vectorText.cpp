#include "vectorText.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "../../core/MNOGLA_util.h"
namespace MNOGLA {
using ::std::vector;
void vectorText::init() {
    twoDShape::init();

    // === buffers ===
    GLCHK(glGenBuffers(1, &vertexBuf));
}

void vectorText::deinit() {
    twoDShape::deinit();
    GLCHK(glDeleteBuffers(1, &vertexBuf));
}

/*static!*/ void vectorText::draw(const ::glm::vec2& pt, const ::std::string& text, float height, const ::glm::vec3& rgb, const ::glm::mat3& world2screen) {
    const vector<glm::vec2> v = text2lines(pt, text, height);
    GLCHK(glUseProgram(p0));

    GLCHK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuf));
    GLCHK(glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(v[0]), &v[0], GL_STATIC_DRAW));

    GLCHK(glEnableVertexAttribArray(p0_coord2d));  // uses currently bound vertex array object
    GLCHK(glVertexAttribPointer(p0_coord2d, /*xy*/ 2, GL_FLOAT, GL_FALSE, 0, NULL));
    GLCHK(glVertexAttribPointer(p0, 2, GL_FLOAT, GL_FALSE, 0, 0));

    GLCHK(glUniform3f(p0_rgb, rgb.r, rgb.g, rgb.b));

    // === mapping ===
    setWorld2screen(world2screen);

    glDrawArrays(GL_LINES, 0, v.size());

    // === cleanup ===
    GLCHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCHK(glDisableVertexAttribArray(p0_coord2d));
    GLCHK(glUseProgram(0));
}

GLuint vectorText::vertexBuf;

#include "rowmans.h"
/*static*/ vector<glm::vec2> vectorText::text2lines(const ::glm::vec2& pt, const std::string& text, float height) {
    vector<glm::vec2> r;
    float curX = 0;
    float f = height / (float)rowmans_height;
    const char* t = text.c_str();
    while (*t != 0) {
        int glyphIx = *t - 32;
        ++t;
        assert(glyphIx >= 0);
        assert(glyphIx < rowmans_count);

        const char* data = rowmans[glyphIx];
        char nNums = rowmans_size[glyphIx];
        float glyphWidth = rowmans_width[glyphIx];
        for (int ix = 0; ix < nNums; ix += 4) {
            float x1 = (float)data[ix + 0];
            float y1 = (float)data[ix + 1];
            float x2 = (float)data[ix + 2];
            float y2 = (float)data[ix + 3];

            x1 += curX;
            x2 += curX;

            r.push_back(glm::vec2(f * x1, f * y1) + pt);
            r.push_back(glm::vec2(f * x2, f * y2) + pt);
        }
        curX += glyphWidth;
    }
    return r;
}

// === API ===
void init_vectorText() {
    vectorText::init();
}

void deinit_vectorText() {
    vectorText::deinit();
}

}  // namespace MNOGLA