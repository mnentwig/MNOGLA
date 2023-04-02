#include <glm/vec2.hpp>
#include <string>

#include "../MNOGLA.h"  // freetype
#include "../gui/guiElem.hpp"
namespace MNOGLA {
using ::std::string, ::glm::vec2, ::glm::vec3;
class multilineText : public guiElem {
   public:
    static void initGlContext() {
        // Note: In case of GL context loss, initGlContext() will be called repeatedly. If so, there is no need to glDelete() anything.
        const char* vShader =
            "#version 300 es\n"
            "in vec4 vertex; // <vec2 pos, vec2 tex>\n"
            "out vec2 texCoords;\n"
            "uniform mat3 world2NDC;\n"
            "void main(){\n"
            "   gl_Position = vec4(world2NDC * vec3(vertex.xy, 1.0f), 1.0f);\n"
            "   texCoords = vertex.zw;\n"
            "}\n";

        // RGB as uniform here?
        const char* fragShader =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 texCoords;\n"
            "out vec4 color;\n"
            "uniform sampler2D text;\n"
            "uniform vec3 rgb;\n"
            "void main(){\n"
            "   color = vec4(rgb, texture(text, texCoords).r);\n"
            "}\n";

        p0 = createProgram(vShader, fragShader);
        p0_vertex = getAttribLoc(p0, "vertex");
        p0_rgb = getUniformLoc(p0, "rgb");
        p0_world2NDC = getUniformLoc(p0, "world2NDC");
        canClean = true;
    }

    multilineText(const glm::vec2& topLeft, float fontsize, const string& text, const glm::vec3& rgb) : rgb(rgb), topLeft(topLeft), fontsize(fontsize) {
    }
    void render(MNOGLA::twoDView& v) override {
        FT_Set_Pixel_Sizes(MNOGLA::freetypeDefaultFace, 0, 900);
        if (FT_Load_Char(MNOGLA::freetypeDefaultFace, 'A', FT_LOAD_RENDER)) throw runtime_error("loadchar failed");

        // === config ===
        GLCHK(glDisable(GL_DEPTH_TEST));
        GLCHK(glUseProgram(p0));

        GLuint texture;
        GLCHK(glGenTextures(1, &texture));
        GLCHK(glBindTexture(GL_TEXTURE_2D, texture));
        GLCHK(glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            MNOGLA::freetypeDefaultFace->glyph->bitmap.width,
            MNOGLA::freetypeDefaultFace->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            MNOGLA::freetypeDefaultFace->glyph->bitmap.buffer));

        GLCHK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCHK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCHK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GLCHK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

        GLCHK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
        GLCHK(glEnable(GL_BLEND));
        GLCHK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        unsigned int VAO, VBO;
        GLCHK(glGenVertexArrays(1, &VAO));
        GLCHK(glGenBuffers(1, &VBO));
        GLCHK(glBindVertexArray(VAO));
        GLCHK(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GLCHK(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW));
        GLCHK(glEnableVertexAttribArray(p0_vertex));
        GLCHK(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0));
        GLCHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCHK(glBindVertexArray(0));

        GLCHK(glUniform3f(p0_rgb, rgb.r, rgb.g, rgb.b));
        GLCHK(glActiveTexture(GL_TEXTURE0));
        GLCHK(glBindVertexArray(VAO));

        // a--d
        // |\ |
        // | \|
        // b--c
        const vec2 a_tex(0.0f, 0.0f);
        const vec2 b_tex(0.0f, 1.0f);
        const vec2 c_tex(1.0f, 1.0f);
        const vec2 d_tex(1.0f, 0.0f);

        float scale = 300;
        const vec2 a_NDC = a_tex * scale;
        const vec2 b_NDC = b_tex * scale;
        const vec2 c_NDC = c_tex * scale;
        const vec2 d_NDC = d_tex * scale;

        vec2 buf[] = {a_NDC, a_tex,
                      b_NDC, b_tex,
                      c_NDC, c_tex,
                      c_NDC, c_tex,
                      d_NDC, d_tex,
                      a_NDC, a_tex};

        GLCHK(glBindTexture(GL_TEXTURE_2D, texture));
        // update content of VBO memory
        GLCHK(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GLCHK(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(buf), buf));
        GLCHK(glBindBuffer(GL_ARRAY_BUFFER, 0));

        GLCHK(glUniformMatrix3fv(p0_world2NDC, /*num matrices*/ 1, /*transpose*/ false, &v.getWorld2screen()[0][0]));

        // render quad
        GLCHK(glDrawArrays(GL_TRIANGLES, 0, 6));

        GLCHK(glBindVertexArray(0));
        GLCHK(glBindTexture(GL_TEXTURE_2D, 0));
        GLCHK(glUseProgram(0));
    }

    void setCol(const vec3& rgb) {
        this->rgb = rgb;
    }
    vector<vec2> getPts() const override {
        return vector<vec2>{topLeft, topLeft + vec2(100.0, 100.0)};  // TBD
    }

   protected:
    static GLuint p0;
    static GLuint p0_vertex;
    static GLuint p0_rgb;
    static GLuint p0_world2NDC;

    static bool canClean;
    vec3 rgb;
    vec2 topLeft;
    float fontsize;
};
GLuint multilineText::p0;
GLuint multilineText::p0_rgb;
GLuint multilineText::p0_vertex;
GLuint multilineText::p0_world2NDC;
bool multilineText::canClean;

}  // namespace MNOGLA