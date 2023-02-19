#include <cmath>
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <stdexcept>

#include "../MNOGLA.h"
#include "../core/MNOGLA_util.h"
#include "../uiEvtListener/ptrEvtListener.h"
#include "../uiEvtListener/src/ptrEvtListener.cpp"
#include "../uiEvtListener/src/rawMouseEvtListener.cpp"
#include "../uiEvtListener/src/rawTouchEvtListener.cpp"
#include "../twoD/twoDView.h"
using std::runtime_error;

class myPtrEvtListener : public ptrEvtListener {
    void evtPtr_preClick(int32_t x, int32_t y) { MNOGLA::logI("pre-click %d %d", x, y); };
    void evtPtr_secondary(int32_t x, int32_t y) { MNOGLA::logI("secondary click %d %d", x, y); };
    void evtPtr_confirmClick(int32_t x, int32_t y) { MNOGLA::logI("confirm click %d %d", x, y); };
    void evtPtr_cancelClick() { MNOGLA::logI("cancel click"); };
};
std::shared_ptr<myPtrEvtListener> evtListener(nullptr);

const bool trace = false;
class myAppState_t {
   public:
    myAppState_t() {
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

   protected:
    GLuint gProgram;
    GLuint gvPositionHandle;
    int appW = -1;
    int appH = -1;
};

void myAppState_t::eventDispatcher() {
#if false
    std::shared_ptr<myAppState_t> as(appState);
    auto x = [as]() {};
    x();
    vector<std::function<void()>> funs;
    funs.push_back(x);
#endif
    int32_t buf[16];
    while (true) {
        size_t n = MNOGLA::evtGetHostToApp(buf);
        if (!n) break;
        assert(evtListener);
        if (evtListener->feedEvtPtr(n, buf)) continue;

        uint32_t key = buf[0];
        switch (key) {
            case MNOGLA::eKeyToHost::WINSIZE:
                appW = buf[1];
                appH = buf[2];
                glViewport(0, 0, appW, appH);  // global (could move to lower-level render function)
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

void myAppState_t::render() {
    if ((appW < 0) || (appH < 0)) throw runtime_error("window size not initialized");
    const glm::vec2 screenWH(appW, appH);
    MNOGLA::twoDView v(0, 0, appW, appH, /*absolutePt2*/ true);
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

    if (trace) MNOGLA::logI("videoCbT0: drawing rect");
    float w = 10.0f;
    glm::vec2 ptC(w, w);
    glm::vec2 ptD(appW - w, appH - w);
    glm::vec3 rgb2(0.0, 1.0, 1.0);
    v.outlinedRect(ptC, ptD, w, rgb2);
}

std::shared_ptr<myAppState_t> myAppState;
void MNOGLA_userInit(int w, int h) {
    myAppState = std::make_shared<myAppState_t>();
    MNOGLA::logI("user init");
    evtListener = std::make_shared<myPtrEvtListener>();
    assert(evtListener);
    //    std::shared_ptr<uiListener> l = std::make_shared<myBasicUiListener>();
    // defer handling the initial size to the resize handler by creating an event
    MNOGLA::evtSubmitHostToApp(MNOGLA::eKeyToHost::WINSIZE, 2, (int32_t)w, (int32_t)h);

    glEnable(GL_DEPTH_TEST);
    MNOGLA::checkGlError("gldepthtest");
    glEnable(GL_BLEND);
    MNOGLA::checkGlError("glblend");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    MNOGLA::checkGlError("glblendfun");
}

static const GLfloat gTriangleVertices[] = {0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f};

void MNOGLA_videoCbT0() {
    if (trace) MNOGLA::logI("videoCbT0: eventDispatcher");

    myAppState->eventDispatcher();
    myAppState->render();

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