#include "MNOGLA_util.h"

#include <stdexcept>
#include <string>
#include <vector>

using std::string, std::runtime_error, std::to_string;

namespace MNOGLA {
void checkGlError(const char* op) {
    GLenum error = glGetError();
    if (!error)
        return;
    string e("GL error (" + string(op) + "):" + to_string(error));
    while ((error = glGetError()) != 0)
        e = e + ", " + to_string(error);
    logE("%s", e.c_str());
    throw runtime_error(e);
}

static GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &pSource, nullptr);
    glCompileShader(shader);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        assert(infoLen);
        char* buf = (char*)malloc(infoLen);
        assert(buf);
        glGetShaderInfoLog(shader, infoLen, nullptr, buf);
        string e("Failed to load GL shader (" + string(shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") + "): " + string(buf));
        logE("%s", e.c_str());
        free(buf);
        throw std::runtime_error(e);
    }
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);

    GLuint program = glCreateProgram();
    if (!program) throw std::runtime_error("glCreateProgram() failed");
    glAttachShader(program, vertexShader);
    checkGlError("glAttachShader");
    glAttachShader(program, pixelShader);
    checkGlError("glAttachShader");
    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLint bufLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
        assert(bufLength);
        char* buf = (char*)malloc(bufLength);
        assert(buf);
        glGetProgramInfoLog(program, bufLength, nullptr, buf);
        string e("failed to link GL program: " + string(buf));
        logE("%s", e.c_str());
        free(buf);
        throw std::runtime_error(e);
    }
    return program;
}

GLint getArgLoc(GLuint prog, const char* argName) {
    GLint r = glGetAttribLocation(prog, argName);
    checkGlError((std::string("glGetAttribLocation:") + argName).c_str());
    if (r < 0) throw runtime_error(std::string("failed to getAttribLocation ") + argName);
    return r;
}

// we don't create an internal header file to only include those few scattered function => declare here
void init_filledRect();
void deinit_filledRect();

void init_outlinedRect();
void deinit_outlinedRect();

void init_vectorText();
void deinit_vectorText();

void initUtil() {
    init_filledRect();
    init_vectorText();
    init_outlinedRect();
}
void deinitUtil() {
    deinit_outlinedRect();
    deinit_vectorText();
    deinit_filledRect();
}

void haltIfGlError(const char* sourceExpr, const char* sourcefile, int sourceline) {
    GLenum err = glGetError();
    if (!err) return;
    std::vector<std::string> errList;
    while (err && (errList.size() < 20)) {
        errList.push_back(std::string(
                              (err == GL_INVALID_ENUM)                    ? "INVALID_ENUM"
                              : (err == GL_INVALID_VALUE)                 ? "INVALID_VALUE"
                              : (err == GL_INVALID_OPERATION)             ? "INVALID_OPERATION"
                              : (err == GL_INVALID_FRAMEBUFFER_OPERATION) ? "INVALID_FRAMEBUFFER_OPERATION"
                              : (err == GL_OUT_OF_MEMORY)                 ? "OUT_OF_MEMORY"
                                                          //                              : (err == GL_STACK_UNDERFLOW)                   ? "STACK_UNDERFLOW"                 // higher GL version
                                                          //                              : (err == GL_STACK_OVERFLOW)                    ? "STACK_OVERFLOW"                  // higher GL version
                                                          //                              : (err == GL_INVALID_FRAMEBUFFER_OPERATION_EXT) ? "INVALID_FRAMEBUFFER_OPERATION"   // higher GL version
                                                          : "unknown") +
                          " (glGetError()==" + std::to_string(err) + ")");
        err = glGetError();
    }  // while more errors
    std::string r;
    for (auto s : errList)
        if (r.size() == 0)
            r = s;
        else
            r = r + ";" + s;
    r = "glError: " + r + " at " + std::string(sourcefile) + " (line " + std::to_string(sourceline) + "): " + std::string(sourceExpr);
    ::MNOGLA::logE("%s", r.c_str());
    throw std::runtime_error(r);
}
}  // namespace MNOGLA

#include "../twoD/code.cpp" // for now...