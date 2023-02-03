#pragma once

// ============================================================================
// load openGl
// ============================================================================
#ifdef MNOGLA_WINDOWS
// Windows version uses glew as it comes with its own copy of relevant headers
// see also host side main.cpp (requires runtime initialization)
#include <GL/glew.h>
#else
#include <GLES3/gl31.h>
#endif
#include <cassert>
#include <stdexcept>

#include "../MNOGLA.h"

static void checkGlError(const char* op) {
    for (GLenum error = glGetError(); error; error = glGetError()) {
        logI("after %s() glError (0x%x)\n", op, error);
    }
}

static GLuint loadShader(GLenum shaderType, const char* pSource, logFun_t logE) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
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
            logE("Could not compile shader %d:\n%s\n", shaderType, buf);
            free(buf);
            throw std::runtime_error("failed to compile shader");
        }
    }
    return shader;
}

static GLuint createProgram(const char* pVertexSource, const char* pFragmentSource, logFun_t logE) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource, logE);
    if (!vertexShader) throw std::runtime_error("failed to create vertex shader");

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource, logE);
    if (!pixelShader) throw std::runtime_error("failed to create pixel shader");

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
        logE("Could not link program:\n%s\n", buf);
        free(buf);
        throw std::runtime_error("failed to link program");
    }
    return program;
}