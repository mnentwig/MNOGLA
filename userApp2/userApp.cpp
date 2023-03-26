#include <cmath>
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <stdexcept>

#include "../MNOGLA.h"
#include "../core/MNOGLA_util.h"
#include "../gui/guiContainer.h"
#include "../twoD/twoDView.h"
using std::runtime_error;

const bool trace = true;
class myAppState_t {
   public:
    myAppState_t() : view(), appW(-1), appH(-1) {
        pGui = std::make_shared<MNOGLA::guiContainer>();
        for (int ix = 3; ix < 5; ++ix) {
            auto b = pGui->button(10, 50 * ix, 500, 45, "hello" + std::to_string(ix));
            b->setClickCallback([ix]() { MNOGLA::logI("hello I am button %i", ix); });
        }
        pGui->freeze();
    }
    void eventDispatcher();
    void render();
    MNOGLA::twoDView view;

   protected:
    int appW;
    int appH;
    ::std::shared_ptr<MNOGLA::guiContainer> pGui = nullptr;
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
            default:
                break;
        }

        MNOGLA::logI("%llu", MNOGLA::lastTimestamp_nanosecs);
    }
}

void myAppState_t::render() {
    if ((appW < 0) || (appH < 0)) throw runtime_error("window size not initialized");
    if (trace) MNOGLA::logI("videoCbT0: glClear");
    GLCHK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GLCHK(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
    
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

std::shared_ptr<myAppState_t> myAppState;
void MNOGLA_userInit(int w, int h) {
    myAppState = std::make_shared<myAppState_t>();
    MNOGLA::logI("user init");

    // defer handling the initial size to the resize handler by creating an event
    MNOGLA::evtSubmitHostToApp(MNOGLA::eKeyToHost::WINSIZE, 2, (int32_t)w, (int32_t)h);

    GLCHK(glEnable(GL_DEPTH_TEST));
    GLCHK(glEnable(GL_BLEND));
    GLCHK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

void MNOGLA_videoCbT0() {
    if (trace) MNOGLA::logI("videoCbT0: eventDispatcher");

    myAppState->eventDispatcher();
    myAppState->render();

    if (trace) MNOGLA::logI("videoCbT0: frame done");
}