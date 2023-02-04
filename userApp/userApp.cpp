#include <cmath>
#include <stdexcept>

#include "../MNOGLA.h"
#include "../core/MNOGLA_util.h"

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
    MNOGLA::evtSubmitHostToApp(MNOGLA::eKeyToHost::WINSIZE, 2, (int32_t)w, (int32_t)h);

    gProgram = MNOGLA::createProgram(gVertexShader, gFragmentShader);
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    MNOGLA::checkGlError("glGetAttribLocation");
}

void eventDispatcher() {
    int32_t buf[16];
    while (true) {
        size_t n = MNOGLA::evtGetHostToApp(buf);
        uint32_t key = buf[0];
        if (!n) break;
        switch (key) {
            case MNOGLA::eKeyToHost::WINSIZE:
                glViewport(0, 0, buf[1], buf[2]);
                continue;
            default:
                break;
        }

        switch (n) {
            case 1:
                MNOGLA::logI("EVT%d\t%d", n, key);
                break;
            case 2:
                MNOGLA::logI("EVT%d\t%d\t%d", n, key, buf[1]);
                break;
            case 3:
                MNOGLA::logI("EVT%d\t%d\t%d\t%d", n, key, buf[1], buf[2]);
                break;
            case 4:
                MNOGLA::logI("EVT%d\t%d\t%d\t%d\t%d", n, key, buf[1], buf[2], buf[3]);
                break;
            default:
                MNOGLA::logI("EVT%d", n);
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
    MNOGLA::checkGlError("glClearColor");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    MNOGLA::checkGlError("glClear");

    glUseProgram(gProgram);
    MNOGLA::checkGlError("glUseProgram");

    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0,
                          gTriangleVertices);
    MNOGLA::checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(gvPositionHandle);
    MNOGLA::checkGlError("glEnableVertexAttribArray");
    glDrawArrays(GL_TRIANGLES, 0, 3);
    MNOGLA::checkGlError("glDrawArrays");
    MNOGLA::xy_t ptA = {0.1f, 0.1f};
    MNOGLA::xy_t ptB = {0.3f, 0.4f};
    MNOGLA::rgb_t col = {0.0f, 1.0f, 0.0f};
    MNOGLA::filledRect::draw(ptA, ptB, col);
}

float vol = 0;
float freq = 0;
void MNOGLA_audioCbT1(float* audioBuf, int32_t numFrames) {
    const float twoPi = 2.0 * 3.14159265358979323846f;
    static float phi = 0;
    float dPhi = freq / 48000.0f * twoPi;
    for (size_t ix = 0; ix < (size_t)numFrames; ++ix) {
        *(audioBuf++) = vol * cos(phi);
        phi += dPhi;
    }
    int n = (int)(phi / twoPi);
    phi -= (float)(n * twoPi);
}

void MNOGLA_midiCbT2(int32_t v0, int32_t v1, int32_t v2) {
    MNOGLA::logI("%02x %02x %02x", v0, v1, v2);
    if ((v0 == 0x90) && (v2 > 0)) {
        vol = 0.1;
        freq = 440.0f * std::pow(2.0f, (v1 - 69) / 12.0f);
    } else {
        vol = 0;
    }
}