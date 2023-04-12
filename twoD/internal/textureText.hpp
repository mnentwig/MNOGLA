#pragma once
#include <glm/vec2.hpp>
#include <iostream>
#include <string>

#include "../../MNOGLA.h"  // freetype
#include "../../core/MNOGLA_util.h"
#include "fontAtlas.h"
#include "twoDShape.h"
namespace MNOGLA {
using ::std::string, ::glm::vec2, ::glm::vec3, ::glm::mat3, ::std::unique_ptr, ::std::make_unique;
class textureText : public twoDShape {
   public:
    static void glInit() {
        // Note: In case of GL context loss, this function glInit() will be called repeatedly.
        // If so, there is no need to glDelete() anything as the GPU memory got wiped completely.
        if (!pAtlas)
            pAtlas = make_unique<MNOGLA::fontAtlas>(MNOGLA::freetypeDefaultFace, 900);

        const char* vShader =
            "#version 300 es\n"
            "in vec4 vertex; // <vec2 pos, vec2 tex>\n"
            "out vec2 texCoords;\n"
            "uniform mat3 world2NDC;\n"
            "void main(){\n"
            "   gl_Position = vec4(world2NDC * vec3(vertex.xy, 1.0f), 1.0f);\n"
            "   texCoords = vertex.zw;\n"
            "}\n";

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

        // === upload bitmap ===
        size_t bmWidth;
        size_t bmHeight;
        uint8_t* bm;
    }

    static void glDeinit() {
        pAtlas = nullptr;
    }

    void draw(const mat3& world2screen, const vec3& rgb) {
        // https://gist.github.com/baines/b0f9e4be04ba4e6f56cab82eef5008ff
        FT_Set_Pixel_Sizes(MNOGLA::freetypeDefaultFace, 0, 900);
        for (unsigned char c = 32; c < 128; ++c) {
            if (!FT_Load_Char(MNOGLA::freetypeDefaultFace, c, FT_LOAD_RENDER)) {
                ::std::cout << MNOGLA::freetypeDefaultFace->glyph->bitmap.rows << "\t" << MNOGLA::freetypeDefaultFace->glyph->bitmap.width << "\n";
                auto s = std::make_shared<FT_GlyphSlot>(MNOGLA::freetypeDefaultFace->glyph);
            }
        }
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
        GLCHK(glBindBuffer(GL_ARRAY_BUFFER, 0));  // TODO: world2screen becomes world2NDC

        GLCHK(glUniformMatrix3fv(p0_world2NDC, /*num matrices*/ 1, /*transpose*/ false, &world2screen[0][0]));

        // render quad
        GLCHK(glDrawArrays(GL_TRIANGLES, 0, 6));

        GLCHK(glBindVertexArray(0));
        GLCHK(glBindTexture(GL_TEXTURE_2D, 0));
        GLCHK(glUseProgram(0));
    }

   protected:
    static GLuint p0;
    static GLuint p0_vertex;
    static GLuint p0_rgb;
    static GLuint p0_world2NDC;

    static bool canClean;
    static unique_ptr<MNOGLA::fontAtlas> pAtlas;
    float fontsize;
};
GLuint textureText::p0;
GLuint textureText::p0_rgb;
GLuint textureText::p0_vertex;
GLuint textureText::p0_world2NDC;
bool textureText::canClean;
unique_ptr<MNOGLA::fontAtlas> textureText::pAtlas = nullptr;

void glInit_textureText() {
    textureText::glInit();
}
void glDeinit_textureText() {
    textureText::glDeinit();
}

}  // namespace MNOGLA