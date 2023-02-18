#include <cmath>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
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
static int appW;
static int appH;
void MNOGLA_userInit(int w, int h) {
    appW = -1;
    appH = -1;  // invalid (used only for sanity check)
    // defer handling the initial size to the resize handler by creating an event
    MNOGLA::evtSubmitHostToApp(MNOGLA::eKeyToHost::WINSIZE, 2, (int32_t)w, (int32_t)h);

    gProgram = MNOGLA::createProgram(gVertexShader, gFragmentShader);
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    MNOGLA::checkGlError("glGetAttribLocation");

    glEnable(GL_DEPTH_TEST);
    MNOGLA::checkGlError("gldepthtest");
    glEnable(GL_BLEND);
    MNOGLA::checkGlError("glblend");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    MNOGLA::checkGlError("glblendfun");
}

void eventDispatcher() {
    int32_t buf[16];
    while (true) {
        size_t n = MNOGLA::evtGetHostToApp(buf);
        uint32_t key = buf[0];
        if (!n) break;
        switch (key) {
            case MNOGLA::eKeyToHost::WINSIZE:
                appW = buf[1];
                appH = buf[2];
                glViewport(0, 0, appW, appH);
                continue;
            default:
                break;
        }

        MNOGLA::logI("%llu", MNOGLA::lastTimestamp_nanosecs);

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
    const bool trace = false;
    if (trace) MNOGLA::logI("videoCbT0: eventDispatcher");

    eventDispatcher();
    if ((appW < 0) || (appH < 0)) throw runtime_error("window size not initialized");
    const glm::vec2 screenWH(appW, appH);
    MNOGLA::twoDView v(0, 0, appW, appH, /*absolutePt2*/true);
    static float grey;
    grey += 0.01f;
    // if (grey > 1.0f) {
    grey = 0.0f;
    //}
    if (trace) MNOGLA::logI("videoCbT0: glClear");
    glClearColor(grey, grey, grey, 1.0f);
    MNOGLA::checkGlError("glClearColor");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    MNOGLA::checkGlError("glClear");

    if (trace) MNOGLA::logI("videoCbT0: filled rect");
    ::glm::vec2 ptA(400, 100);
    ::glm::vec2 ptB(500, 150);
    ::glm::vec3 col(0.0f, 1.0f, 0.0f);
    glm::vec2 screenTopLeft(0, 0);
    v.filledRect(ptA, ptB, col);

#if 1

    if (trace) MNOGLA::logI("videoCbT0: glProg for Tri");
    glUseProgram(gProgram);
    MNOGLA::checkGlError("glUseProgram");

    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0,
                          gTriangleVertices);
    MNOGLA::checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(gvPositionHandle);
    MNOGLA::checkGlError("glEnableVertexAttribArray");
    glDrawArrays(GL_TRIANGLES, 0, 3);
    MNOGLA::checkGlError("glDrawArrays");
    if (trace) MNOGLA::logI("videoCbT0: filled rect done");

    //    for (int c = 0; c <= 4; ++c) {
    //      float oX = (c == 1) ? -1 : (c == 2) ? 1
    //                                        : 0;
    //  float oY = (c == 3) ? -1 : (c == 4) ? 1
    //                                    : 0;
    const glm::vec3 rgb(0.0f, 1.0f, 0.0f);
    for (float row = 0; row < 10; ++row) {
        if (trace) MNOGLA::logI("text row %d start", (int)row);
        float textsize = 60;
        glm::vec2 pos(textsize, row * textsize);
        v.vectorText(pos, "Hello world", textsize, rgb);
        if (trace) MNOGLA::logI("text row %d done", (int)row);
    }
#endif
    if (trace) MNOGLA::logI("videoCbT0: drawing rect");
    float w = 10.0f;
    glm::vec2 ptC(w, w);
    glm::vec2 ptD(appW - w, appH - w);
    glm::vec3 rgb2(0.0, 1.0, 1.0);
    v.outlinedRect(ptC, ptD, w, rgb2);

    if (trace) MNOGLA::logI("videoCbT0: frame done");
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