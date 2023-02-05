#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "MNOGLA_includeGl.h"
class instStackLine {
    public:
    static void init();
   protected:
   static GLuint prog;
    bool isFinalized = false;

    struct myGlLine {
        GLushort v1;
        GLushort v2;
    };

    struct myGlVertex {
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat r;
        GLfloat g;
        GLfloat b;
    };

    std::vector<myGlLine> lineList;
    std::vector<myGlVertex> vertexList;

    // post-finalize
    GLuint idVertex;
    GLuint idLine;

   public:
    instStackLine();
    ~instStackLine();
    unsigned int pushVertex(::glm::vec3 xyz, ::glm::vec3 rgb);
    void pushLine(int v1, int v2);
    void finalize();
    void run(::glm::mat4 proj);
};
void renderText(instStackLine* is, const char* text, glm::vec3 rgb);
glm::mat4 getTextProj2d(glm::vec2 pos, glm::vec2 screenWH, float fontHeight);
