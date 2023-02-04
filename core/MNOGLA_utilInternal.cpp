#include "MNOGLA_utilInternal.h"

#include <string.h>

#include <stdexcept>

#include "MNOGLA_util.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>

#define LOCATION_COORD3D 0
#define LOCATION_RGB 1
#define LOCATION_MVP_COL0 2
#define LOCATION_MVP_COL1 3
#define LOCATION_MVP_COL2 4
#define LOCATION_MVP_COL3 5
#define LOCATION_HALFVIEWPORT 6

void instStackLine::init() {
    auto vs =
        "#version 300 es\n"
        "layout (location = 0) in vec3 coord3d;"
        "layout (location = 1) in vec3 rgb;"
        "layout (location = 2) in vec4 mvp_col0;"
        "layout (location = 3) in vec4 mvp_col1;"
        "layout (location = 4) in vec4 mvp_col2;"
        "layout (location = 5) in vec4 mvp_col3;"
        "layout (location = 6) in vec2 halfViewport;"
        "out vec3 scr;"
        "out vec3 rgbv;"
        "void main(void) {"
        "     mat4 mvp2;"
        "     mvp2[0] = mvp_col0;"
        "     mvp2[1] = mvp_col1;"
        "     mvp2[2] = mvp_col2;"
        "     mvp2[3] = mvp_col3;"
        "     vec4 pos = mvp2 * vec4(coord3d, 1.0);"
        "     gl_Position = pos;"
        "     scr = vec3("
        "     	pos.x*halfViewport.x + halfViewport.x*pos.w,"
        "	pos.y*halfViewport.y + halfViewport.y*pos.w,"
        "	pos.w);"
        "     rgbv = rgb;"
        "}";
    auto fs =
        "#version 300 es\n"
        "precision mediump float;"
        "in vec3 rgbv;"
        "out vec4 fragmentColor;"
        "in vec3 scr;"
        "void main(void) {"
        "     vec2 lcxy = scr.xy / scr.z;"
        "     float d = length(gl_FragCoord.xy - lcxy);"
        "     float linewidth = 1.5f;"
        "     d = d / linewidth;"
        "     float b = 1.0f-d;"
        "     b = min(b, 1.0f);"
        "     b = max(b, 0.0f);"
        "     fragmentColor = vec4(rgbv, b);"
        "     float fragZ_eyeCoord = gl_FragCoord.z / gl_FragCoord.w;"
        "     float zmul = 1.0-((fragZ_eyeCoord-8.0f) / 8.0f);"
        "     zmul = max(zmul, 0.0);"
        "     zmul = min(zmul, 1.0);"
        "     fragmentColor.xyz *= zmul;"
        "}";
    prog = MNOGLA::createProgram(vs, fs);
}
GLuint instStackLine::prog;
instStackLine::instStackLine() {
    this->idLine = 0;    // avoid warning
    this->idVertex = 0;  // avoid warning
}

unsigned int instStackLine::pushVertex(glm::vec3 xyz, glm::vec3 rgb) {
    assert(!this->isFinalized);
    unsigned int retVal = this->vertexList.size();
    myGlVertex v = {xyz.x, xyz.y, xyz.z, rgb.r, rgb.g, rgb.b};
    this->vertexList.push_back(v);
    return retVal;
}

void instStackLine::pushLine(int v1, int v2) {
    assert(!this->isFinalized);
    this->lineList.push_back({(GLushort)v1, (GLushort)v2});
}

void instStackLine::finalize() {
    assert(!this->isFinalized);
    this->isFinalized = true;

    glGenBuffers(1, &this->idVertex);
    MNOGLA::checkGlError("genBuf vertex");
    glBindBuffer(GL_ARRAY_BUFFER, this->idVertex);
    MNOGLA::checkGlError("bindBuf vertex");
    glBufferData(GL_ARRAY_BUFFER, this->vertexList.size() * sizeof(this->vertexList[0]), &this->vertexList[0], GL_STATIC_DRAW);
    MNOGLA::checkGlError("bufData vertex");

    glGenBuffers(1, &this->idLine);
    MNOGLA::checkGlError("genBuf line");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idLine);
    MNOGLA::checkGlError("bindBuf line");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->lineList.size() * sizeof(this->lineList[0]), &this->lineList[0], GL_STATIC_DRAW);
    MNOGLA::checkGlError("bufData line");

    // === cleanup ===
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void instStackLine::run(glm::mat4 proj, float viewportX, float viewportY) {
    assert(this->isFinalized);

    glLineWidth(5.0);
    MNOGLA::checkGlError("glLineWidth");
    glEnableVertexAttribArray(LOCATION_COORD3D);
    MNOGLA::checkGlError("enableVertAttrArr coord3d");
    glEnableVertexAttribArray(LOCATION_RGB);
    MNOGLA::checkGlError("enableVertAttrArr rgb");

    glBindBuffer(GL_ARRAY_BUFFER, this->idVertex);
    glVertexAttribPointer(LOCATION_COORD3D, 3, GL_FLOAT, GL_FALSE, sizeof(myGlVertex), 0);
    MNOGLA::checkGlError("vaptr coord3d");
    glVertexAttribDivisor(LOCATION_COORD3D, 0);
    MNOGLA::checkGlError("vaDiv coord3d");
    glVertexAttribPointer(LOCATION_RGB, 3, GL_FLOAT, GL_FALSE, sizeof(myGlVertex), (void*)(3 * sizeof(GLfloat)));
    MNOGLA::checkGlError("vaptr rgb");
    glVertexAttribDivisor(LOCATION_RGB, 0);
    MNOGLA::checkGlError("vaDiv rgb");

    glVertexAttrib4f(LOCATION_MVP_COL0, proj[0][0], proj[0][1], proj[0][2], proj[0][3]);
    MNOGLA::checkGlError("va4f");
    glVertexAttrib4f(LOCATION_MVP_COL1, proj[1][0], proj[1][1], proj[1][2], proj[1][3]);
    MNOGLA::checkGlError("va4f");
    glVertexAttrib4f(LOCATION_MVP_COL2, proj[2][0], proj[2][1], proj[2][2], proj[2][3]);
    MNOGLA::checkGlError("va4f");
    glVertexAttrib4f(LOCATION_MVP_COL3, proj[3][0], proj[3][1], proj[3][2], proj[3][3]);
    MNOGLA::checkGlError("va4f");
    glVertexAttrib2f(LOCATION_HALFVIEWPORT, viewportX / 2.0f, viewportY / 2.0f);
    MNOGLA::checkGlError("va4f");

    glEnable(GL_POLYGON_OFFSET_FILL);
    MNOGLA::checkGlError("polyOffsetFill");
    glPolygonOffset(1.0f, 1.0f);
    MNOGLA::checkGlError("polyOffset");

    // draw outlines in green
    glUseProgram(this->prog);
    MNOGLA::checkGlError("glProg");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idLine);
    MNOGLA::checkGlError("glBindBuf");
    glDrawElements(GL_LINES, this->lineList.size() * 2, GL_UNSIGNED_SHORT, 0);
    MNOGLA::checkGlError("glDrawElem");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    MNOGLA::checkGlError("bindBuf 0");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    MNOGLA::checkGlError("bindBuf 0");
    glDisableVertexAttribArray(LOCATION_COORD3D);
    MNOGLA::checkGlError("disVertAttr");
    glUseProgram(0);
    MNOGLA::checkGlError("useProg 0");
}

instStackLine::~instStackLine() {
    if (this->isFinalized) {
        glDeleteBuffers(1, &this->idLine);
        glDeleteBuffers(1, &this->idVertex);
    }
}

#include "rowmans.h"
void renderText(instStackLine* is, const char* text, glm::vec3 rgb){
  float curX = 0;
  float f = 1.0/(float)rowmans_height;
  while (*text != 0){
    int glyphIx = *text - 32;
    ++text;
    assert(glyphIx >= 0);
    assert(glyphIx < rowmans_count);

    const char* data = rowmans[glyphIx];
    char nNums = rowmans_size[glyphIx];
    float glyphWidth = rowmans_width[glyphIx];
    for (int ix = 0; ix < nNums; ix += 4){
      float x1 = (float)data[ix+0];
      float y1 = -(float)data[ix+1];
      float x2 = (float)data[ix+2];
      float y2 = -(float)data[ix+3];
      
      x1 += curX;
      x2 += curX;

      const float z = 0.0f;
      int v1 = is->pushVertex(glm::vec3(f*x1, f*y1, z), rgb);
      int v2 = is->pushVertex(glm::vec3(f*x2, f*y2, z), rgb);
      is->pushLine(v1, v2);
    }
    curX += glyphWidth;
  }
}