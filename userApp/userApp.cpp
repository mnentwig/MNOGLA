#include <cmath>  //M_PI
#include <stdexcept>

#include "../util/util.hpp"

using std::runtime_error;

auto gVertexShader =
    "attribute vec4 vPosition;\n"
    "void main() {\n"
    "  gl_Position = vPosition;\n"
    "}\n";

auto gFragmentShader =
    "precision mediump float;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);\n"
    "}\n";

static GLuint gProgram = 0;
static GLuint gvPositionHandle;
void MNOGLA_userInit(int w, int h) {
    // defer handling the initial size to the resize handler by creating an event
    MNOGLA_evtSubmitHostToApp(MNOGLA_eKeyToHost::WINSIZE, 2, (int32_t)w, (int32_t)h);

    gProgram = createProgram(gVertexShader, gFragmentShader, logE);
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    checkGlError("glGetAttribLocation");
    logI("glGetAttribLocation(\"vPosition\") = %d\n", gvPositionHandle);
}

void eventDispatcher() {
    int32_t buf[16];
    while (true) {
        size_t n = MNOGLA_evtGetHostToApp(buf);
        uint32_t key = buf[0];
        if (!n) break;
        switch (key) {
            case MNOGLA_eKeyToHost::WINSIZE:
                glViewport(0, 0, buf[1], buf[2]);
                continue;
            default:
                break;
        }

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
}

static const GLfloat gTriangleVertices[] = {0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f};

void MNOGLA_videoCbT0() {
    eventDispatcher();

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
void MNOGLA_audioCbT1(float* audioBuf, int32_t numFrames) {
    static float phi = 0;
    float dPhi = freq / 48000.0f * 2.0f * M_PI;
    for (size_t ix = 0; ix < (size_t)numFrames; ++ix) {
        *(audioBuf++) = vol * cos(phi);
        phi += dPhi;
    }
    int n = (int)(phi / (2 * M_PI));
    phi -= (float)(n * (2 * M_PI));
}

void MNOGLA_midiCbT2(int32_t v0, int32_t v1, int32_t v2) {
    logI("%02x %02x %02x", v0, v1, v2);
    if ((v0 == 0x90) && (v2 > 0)) {
        vol = 0.1;
        freq = 440.0f * std::pow(2.0f, (v1 - 69) / 12.0f);
    } else {
        vol = 0;
    }
}