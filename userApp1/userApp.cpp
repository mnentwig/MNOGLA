#include <cmath>
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <stdexcept>

#include "../MNOGLA.h"
#include "../core/MNOGLA_util.h"
#include "../gui/guiButton.hpp"
#include "../gui/guiContainer.h"
#include "../twoD/twoDView.h"
#include "synth/audiobook.hpp"
using std::runtime_error;

const bool trace = false;
class myAppState_t {
   public:
    myAppState_t() : view(), appW(-1), appH(-1) {
        pGui = std::make_shared<MNOGLA::guiContainer>();
        for (int ix = 3; ix < 13; ++ix) {
            auto b = ::std::make_shared<MNOGLA::guiButton>(10, 50 * ix, 500, 45, "hello" + std::to_string(ix));
            b->setClickCallback([ix]() { MNOGLA::logI("hello I am button %i", ix); });
            pGui->addElem(b);
        }
        pGui->freeze();

        vector<int> steps;
        int basenote = 52;
        for (int oct = -2; oct <= 1; ++oct) {
            steps.push_back(12 * oct + basenote + 0);
            steps.push_back(12 * oct + basenote + 3);
            steps.push_back(12 * oct + basenote + 7);
            steps.push_back(12 * oct + basenote + 10);
        }
        for (int oct = 1; oct >= -2; --oct) {
            steps.push_back(12 * oct + basenote + 12);
            steps.push_back(12 * oct + basenote + 10);
            steps.push_back(12 * oct + basenote + 7);
            steps.push_back(12 * oct + basenote + 3);
        }
    }

    // runs on startup and when glContext is re-acquired e.g. Android wakes from sleep and GPU mem was wiped
    void initGlContext() {
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
        gProgram = MNOGLA::createProgram(gVertexShader, gFragmentShader);
        gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
        MNOGLA::checkGlError("glGetAttribLocation");
    }
    void eventDispatcher();
    void render();
    void startAudio(int32_t nChan, int32_t rate_Hz) {
        MNOGLA::mono1::config(1, 1.0f / rate_Hz);
        abook = ::std::make_shared<MNOGLA::audiobook>("hello.ods", (float)rate_Hz);
    }
    void runAudio(float* dest, size_t nFrames) {
        if (!abook) return;
        abook->run(dest, nFrames);
    }
    MNOGLA::twoDView view;

   protected:
    GLuint gProgram = 0;
    GLuint gvPositionHandle = 0;
    int appW;
    int appH;
    ::std::shared_ptr<MNOGLA::guiContainer> pGui = nullptr;
    ::std::shared_ptr<MNOGLA::audiobook> abook = nullptr;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static void logRawEvents(int32_t n, const int32_t* buf) {
    const uint32_t key = buf[0];
    if (key == MNOGLA::eKeyToHost::TOUCH_MOVE) return;
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
#pragma GCC diagnostic pop

void myAppState_t::eventDispatcher() {
    int32_t buf[16];
    while (true) {
        const size_t n = MNOGLA::evtGetHostToApp(buf);
        if (!n) break;
        // logRawEvents(n, buf);
        if (pGui->feedEvtPtr(n, buf)) continue;

        const uint32_t key = buf[0];
        switch (key) {
            case MNOGLA::eKeyToHost::WINSIZE: {
                appW = buf[1];
                appH = buf[2];

                glm::vec2 topLeft = glm::vec2(0, 0);
                const glm::vec2 screenWH(appW, appH);
                glm::vec2 center = topLeft + screenWH / 2.0f;
                view.set(center, screenWH, 0.0f);
                pGui->informViewport(0, 0, appW, appH);
                pGui->autoscale();

                glViewport(0, 0, appW, appH);  // global (could move to lower-level render function)
                continue;
            }
            case MNOGLA::eKeyToHost::AUDIO_START: {
                int32_t nChan = buf[1];
                int32_t audiorate_Hz = buf[2];
                startAudio(nChan, audiorate_Hz);
            }
            default:
                break;
        }

        MNOGLA::logI("%llu", MNOGLA::lastTimestamp_nanosecs);
    }
}

void myAppState_t::render() {
    if ((appW < 0) || (appH < 0)) throw runtime_error("window size not initialized");
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
    view.filledRect(ptA, ptB, col);

    glUseProgram(gProgram);
    MNOGLA::checkGlError("glUseProgram");

    const GLfloat gTriangleVertices[] = {0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f};
    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0,
                          gTriangleVertices);
    MNOGLA::checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(gvPositionHandle);
    MNOGLA::checkGlError("glEnableVertexAttribArray");
    glDrawArrays(GL_TRIANGLES, 0, 3);
    MNOGLA::checkGlError("glDrawArrays");
    if (trace) MNOGLA::logI("videoCbT0: filled rect done");

    const glm::vec3 rgb(0.0f, 1.0f, 0.0f);
    for (float row = 0; row < 10; ++row) {
        if (trace) MNOGLA::logI("text row %d start", (int)row);
        float textsize = 60;
        glm::vec2 pos(textsize, row * textsize);
        view.vectorText(pos, "Hello world", textsize, rgb);
        if (trace) MNOGLA::logI("text row %d done", (int)row);
    }

    if (trace) MNOGLA::logI("videoCbT0: drawing rect");
    float w = 10.0f;
    glm::vec2 ptC(w, w);
    glm::vec2 ptD(appW - w, appH - w);
    glm::vec3 rgb2(0.0, 1.0, 1.0);
    view.outlinedRect(ptC, ptD, w, rgb2);

    pGui->render();
}

std::shared_ptr<myAppState_t> myAppState = nullptr;
void MNOGLA_userInit() {
    myAppState = std::make_shared<myAppState_t>();
    MNOGLA::logI("user init");
}

void MNOGLA_initGlContext() {
    glEnable(GL_DEPTH_TEST);
    MNOGLA::checkGlError("gldepthtest");
    glEnable(GL_BLEND);
    MNOGLA::checkGlError("glblend");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    MNOGLA::checkGlError("glblendfun");
    if (!myAppState) throw runtime_error("initGlContext before userInit");
    myAppState->initGlContext();
}

static const GLfloat gTriangleVertices[] = {0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f};

void MNOGLA_videoCbT0() {
    if (trace) MNOGLA::logI("videoCbT0: eventDispatcher");

    myAppState->eventDispatcher();
    myAppState->render();

    if (trace) MNOGLA::logI("videoCbT0: frame done");
}

float vol = 0.1;
float freq = 440;
void MNOGLA_audioCbT1(float* audioBuf, int32_t numFrames) {
    for (int32_t ix = 0; ix < numFrames; ++ix)
        *(audioBuf + ix) = 0.0f;
    myAppState->runAudio(audioBuf, numFrames);
#if 0
    const float twoPi = 2.0 * 3.14159265358979323846f;
    static float phi = 0;
    float dPhi = freq / 48000.0f * twoPi;
    for (size_t ix = 0; ix < (size_t)numFrames; ++ix) {
        *(audioBuf++) = vol * cos(phi);
        phi += dPhi;
    }
    int n = (int)(phi / twoPi);
    phi -= (float)(n * twoPi);
#endif
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