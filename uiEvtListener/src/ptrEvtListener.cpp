#include "../ptrEvtListener.h"

#include <glm/gtx/norm.hpp>  // length2
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../../twoD/twoDMatrix.h"
namespace MNOGLA {
using ::glm::vec2, ::glm::vec3;
ptrEvtListener_internal::ptrEvtListener_internal()
    : validFirstDown(false),
      firstDownPtr(0),
      firstDownPt(0, 0),
      firstDownPtRaw(0, 0),
      config(),
      normalizeMouse(/*identity matrix*/ 1.0f),
      aspectRatio(1.0f) {}
ptrEvtListener::ptrEvtListener() {}

ptrEvtListener::ptrEvtListener(ptrEvtListenerConfig& config) {
    this->config = config;
}

void ptrEvtListener::informViewport(float x, float y, float w, float h) {
    vec2 center(x + w / 2.0f, y + h / 2.0f);
    normalizeMouse = twoDMatrix::scale(vec2(2.0f / w, -2.0f / h)) * twoDMatrix::translate(-center);
    assert(h != 0);
    aspectRatio = w / h;  // conventional screensize-style definition e.g. 16:9
}

bool ptrEvtListener::feedEvtPtr(size_t n, int32_t* buf) {
    return rawMouseEvtListener::feedEvtMouse(n, buf) || rawTouchEvtListener::feedEvtTouch(n, buf) || false;
}

// is point x, y within configuration-defined click radius around firstDownPtRaw in units of raw pixels
bool ptrEvtListener_internal::withinClickRadius(int32_t xRaw, int32_t yRaw) const {
    return ::glm::length2(firstDownPtRaw - vec2(xRaw, yRaw)) <= config.clickRadius_pixels * config.clickRadius_pixels;
}

void ptrEvtListener_internal::evtTouchRaw_down(int32_t ptrNum, int32_t x, int32_t y) {
    MNOGLA::logI("DOWN");
    if (validFirstDown) {
        MNOGLA::logI("DOWNa");
        validFirstDown = false;
        evtPtr_cancelClick();
        return;
    }
    MNOGLA::logI("DOWNb");
    const vec2 ptNorm = normalizeMouse * glm::vec3(x, y, 1.0f);
    firstDownPtr = ptrNum;
    firstDownPt = ptNorm;
    firstDownPtRaw = vec2(x, y);
    validFirstDown = true;
    evtPtr_preClick(ptNorm);
};

void ptrEvtListener_internal::evtTouchRaw_up(int32_t ptrNum, int32_t x, int32_t y) {
    if (!validFirstDown)
        return;
    if (withinClickRadius(x, y)) {
        const vec2 ptNorm = normalizeMouse * vec3(x, y, 1.0f);
        evtPtr_confirmClick(ptNorm);
    } else
        evtPtr_cancelClick();
    validFirstDown = false;
};

void ptrEvtListener_internal::evtTouchRaw_move(int32_t ptrNum, int32_t x, int32_t y) {
    if (!validFirstDown)
        return;

    if (!withinClickRadius(x, y)) {
        validFirstDown = false;
        evtPtr_cancelClick();
    }
}

void ptrEvtListener_internal::evtMouseRaw_down(int32_t bnum) {
    switch (bnum) {
        case 0:
            evtTouchRaw_down(/*ptrNum*/ -1, lastMouseX, lastMouseY);  // fake touchscreen pointer "-1"
            break;
        case 1:
            evtPtr_secondary(getLastMouseNormalized());
            break;
        default:
            break;
    }
}

void ptrEvtListener_internal::evtMouseRaw_up(int32_t bnum) {
    if (bnum == 0) {
        evtTouchRaw_up(/*ptrNum*/ -1, lastMouseX, lastMouseY);
    }
}

void ptrEvtListener_internal::evtMouseRaw_move(int32_t x, int32_t y) {
    rawMouseEvtListener::evtMouseRaw_move(x, y);              // update lastMouseX, lastMouseY
    evtTouchRaw_move(/*ptrNum*/ -1, lastMouseX, lastMouseY);  // fake touchscreen pointer "-1"
}

vec2 ptrEvtListener_internal::getLastMouseNormalized() {
    return normalizeMouse * vec3(lastMouseX, lastMouseY, 1.0f);
}
}  // namespace MNOGLA