#include <stdexcept>

#include "../MNOGLA.h"
#ifdef MNOGLA_WINDOWS
// Windows version uses glew as it comes with its own copy of relevant headers
// see also host side main.cpp (requires runtime initialization)
#include <GL/glew.h>
#else
// include openGLES headers ourselves
#include <GLES3/gl31.h>
#endif

using std::runtime_error;
static logFun_t logI = nullptr;
static logFun_t logE = nullptr;

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
            if (infoLen) {
                char* buf = (char*)malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, nullptr, buf);
                    logE("Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

static GLuint createProgram(const char* pVertexSource, const char* pFragmentSource, logFun_t logE) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource, logE);
    if (!vertexShader) throw runtime_error("failed to create vertex shader");

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource, logE);
    if (!pixelShader) throw runtime_error("failed to create pixel shader");

    GLuint program = glCreateProgram();
    if (!program) throw runtime_error("glCreateProgram() failed");
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
        if (bufLength) {
            char* buf = (char*)malloc(bufLength);
            if (buf) {
                glGetProgramInfoLog(program, bufLength, nullptr, buf);
                logE("Could not link program:\n%s\n", buf);
                free(buf);
            }
        }
        glDeleteProgram(program);
        program = 0;
    }
    return program;
}

auto gVertexShader =
    "attribute vec4 vPosition;\n"
    "void main() {\n"
    "  gl_Position = vPosition;\n"
    "}\n";

auto gFragmentShader =
    "precision mediump float;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(0.7, 0.7, 1.0, 1.0);\n"
    "}\n";

static GLuint gProgram = 0;
static GLuint gvPositionHandle;

void MNOGLA_init(int w, int h, logFun_t _logI, logFun_t _logE) {
    logI = _logI;
    logE = _logE;

#ifdef MNOGLA_WINDOWS
    // Windows version uses GLEW to load openGl libraries but this requires initialization
    // for dynamic loading
    glewExperimental = 1;  // Needed for core profile
    if (glewInit() != GLEW_OK) throw runtime_error("Failed to initialize GLEW");
#endif

    logI("Version", GL_VERSION);
    logI("Vendor", GL_VENDOR);
    logI("Renderer", GL_RENDERER);
    logI("Extensions", GL_EXTENSIONS);

    logI("setupGraphics(%d, %d)", w, h);
    gProgram = createProgram(gVertexShader, gFragmentShader, logE);
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    checkGlError("glGetAttribLocation");
    logI("glGetAttribLocation(\"vPosition\") = %d\n", gvPositionHandle);

    glViewport(0, 0, w, h);
    checkGlError("glViewport");
}

static const GLfloat gTriangleVertices[] = {0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f};

void MNOGLA_videoCbT0() {
    int32_t buf[16];
    while (true) {
        size_t n = MNOGLA_evtGetHostToApp(buf);
        uint32_t key = buf[0];
        if (!n) break;
        switch (n) {
            case 1:
                logI("EVT%d\t%d", n, key);
                break;
            case 2:
                logI("EVT%d\t%d\t%d", n, key, buf[1]);
                break;
            case 3:
                logI("EVT%d\t%d\t%d\t%d", n, key, buf[1], buf[2]);
                break;
            case 4:
                logI("EVT%d\t%d\t%d\t%d\t%d", n, key, buf[1], buf[2], buf[3]);
                break;
            default:
                logI("EVT%d", n);
                break;
        }
    }

    static float grey;
    grey += 0.01f;
    // if (grey > 1.0f) {
    grey = 0.0f;
    //}
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    glUseProgram(gProgram);
    checkGlError("glUseProgram");

    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0,
                          gTriangleVertices);
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(gvPositionHandle);
    checkGlError("glEnableVertexAttribArray");
    glDrawArrays(GL_TRIANGLES, 0, 3);
    checkGlError("glDrawArrays");
}

float vol = 0;
float freq = 0;
void MNOGLA_audioCbT1(float* audioBuf, int32_t numFrames){
  static float phi = 0;
  float dPhi = freq / 48000.0f * 2.0f * M_PI;
  for (size_t ix = 0; ix < numFrames; ++ix) {
    *(audioBuf++) = vol * cos(phi);
    phi += dPhi;
  }
  int n = (int) (phi / (2 * M_PI));
  phi -= (float) (n * (2 * M_PI));
}
#include <cmath>
void MNOGLA_midiCbT2(int32_t v0, int32_t v1, int32_t v2){
  logI("%02x %02x %02x", v0, v1, v2);
  if ((v0 == 0x90) && (v2 > 0)){
    vol = 0.1;
    freq = 440.0f * std::pow(2.0f, (v1-69)/12.0f);
  } else {
    vol=0;
  }
}
