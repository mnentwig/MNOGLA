#include "MNOGLA_util.h"

#include <stdexcept>
#include <string>
using std::string, std::runtime_error, std::to_string;
#include "../MNOGLA.h"

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

GLuint loadShader(GLenum shaderType, const char* pSource, logFun_t logE) {
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
        string e("Failed to load GL shader (" + to_string(shaderType) + "): " + string(buf));
        logE("%s", e.c_str());
        free(buf);
        throw std::runtime_error(e);
    }
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource, logFun_t logE) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource, logE);
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource, logE);

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