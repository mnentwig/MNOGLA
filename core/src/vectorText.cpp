#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "../MNOGLA_util.h"
namespace MNOGLA {
class vectorText {
   public:
    static void init();
    static void deinit();
    static void draw(const ::glm::vec2& pt, const ::std::string& text, float height, const ::glm::vec3& rgb, const ::glm::vec2& screen);

   protected:
    static vector<glm::vec2> text2lines(const ::glm::vec2& pt, const ::std::string& text, float height);
    static GLuint prog;
    static GLuint vertexBuf;
    static GLint argLoc_coord2d;
    static GLint argLoc_rgb;
    static GLint argLoc_scale;
    static GLint argLoc_offset;
};

void vectorText::init() {
    auto vs =
        "#version 300 es\n"
        "layout (location = 0) in vec2 coord2d;\n"
        "layout (location = 1) in vec3 rgb;\n"
        "layout (location = 2) in vec2 scale;\n"
        "layout (location = 3) in vec2 offset;\n"
        "out vec3 rgbv;\n"
        "void main() {\n"
        "  gl_Position = vec4(coord2d.x * scale.x + offset.x, coord2d.y * scale.y + offset.y, 0.0, 1.0);\n"
        "  rgbv = rgb;\n"
        "}\n";
    auto fs =
        "#version 300 es\n"
        "precision mediump float;"
        "in vec3 rgbv;"
        "out vec4 fragmentColor;"
        "void main(void) {"
        "   fragmentColor = vec4(rgbv, 1.0f);"
        "}";

    prog = createProgram(vs, fs);

    // === argument locations ===
    argLoc_coord2d = getArgLoc(prog, "coord2d");
    argLoc_rgb = getArgLoc(prog, "rgb");
    argLoc_scale = getArgLoc(prog, "scale");
    argLoc_offset = getArgLoc(prog, "offset");

    // === buffers ===
    GLCHK(glGenBuffers(1, &vertexBuf));
}

void vectorText::deinit() {
    GLCHK(glDeleteBuffers(1, &vertexBuf));
}

/*static!*/ void vectorText::draw(const ::glm::vec2& pt, const ::std::string& text, float height, const ::glm::vec3& rgb, const ::glm::vec2& screen) {
    const vector<glm::vec2> v = text2lines(pt, text, height);
    GLCHK(glUseProgram(prog));

    GLCHK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuf));
    GLCHK(glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(v[0]), &v[0], GL_STATIC_DRAW));

    GLCHK(glEnableVertexAttribArray(argLoc_coord2d));  // uses currently bound vertex array object
    GLCHK(glVertexAttribPointer(argLoc_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0));

    GLCHK(glDisableVertexAttribArray(argLoc_rgb));
    GLCHK(glVertexAttrib3f(argLoc_rgb, rgb.r, rgb.g, rgb.b));

    // === mapping ===
    GLCHK(glVertexAttrib2f(argLoc_scale, 2.0f / screen.x, -2.0f / screen.y));
    GLCHK(glDisableVertexAttribArray(argLoc_scale));
    GLCHK(glVertexAttrib2f(argLoc_offset, -1.0f, 1.0f));
    GLCHK(glDisableVertexAttribArray(argLoc_offset));

    glDrawArrays(GL_LINES, 0, v.size());

    // === cleanup ===
    GLCHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCHK(glDisableVertexAttribArray(argLoc_coord2d));
    GLCHK(glUseProgram(0));
}

GLuint vectorText::vertexBuf;
GLuint vectorText::prog;
GLint vectorText::argLoc_coord2d;
GLint vectorText::argLoc_rgb;
GLint vectorText::argLoc_scale;
GLint vectorText::argLoc_offset;

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
void draw_vectorText(const ::glm::vec2& pt, const ::std::string& text, float height, const ::glm::vec3& rgb, const ::glm::vec2& screen) {
    vectorText::draw(pt, text, height, rgb, screen);
}

void init_vectorText() {
    vectorText::init();
}

void deinit_vectorText() {
    vectorText::deinit();
}

}  // namespace MNOGLA