#include "../ptrEvtListener.h"

#include <glm/gtx/norm.hpp>

#include "../../twoD/twoDMatrix.h"
namespace MNOGLA {
ptrEvtListener_internal::ptrEvtListener_internal()
    : validFirstDown(false),
      firstDownPtr(0),
      firstDownPt(0, 0),
      config(),
      normalizeMouse(/*identity matrix*/ 1.0f) {}
ptrEvtListener::ptrEvtListener() {}

ptrEvtListener::ptrEvtListener(ptrEvtListenerConfig& config) {
    this->config = config;
}

void ptrEvtListener::informViewport(float x, float y, float w, float h) {
    glm::vec2 center(x + w / 2.0f, y + h / 2.0f);
    normalizeMouse = twoDMatrix::scale(glm::vec2(2.0f / w, -2.0f / h)) * twoDMatrix::translate(-center);
}

bool ptrEvtListener::feedEvtPtr(size_t n, int32_t* buf) {
    return rawMouseEvtListener::feedEvtMouse(n, buf) || rawTouchEvtListener::feedEvtTouch(n, buf) || false;
}

// is point x, y within configuration-defined click radius around firstDownPt?
bool ptrEvtListener_internal::withinClickRadius(const ::glm::vec2& ptNorm) const {
    return ::glm::length2(ptNorm - firstDownPt) <= config.clickRadius_pixels;
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
    const ::glm::vec2 ptNorm = normalizeMouse * glm::vec3(x, y, 1.0f);
    firstDownPtr = ptrNum;
    firstDownPt = ptNorm;
    validFirstDown = true;
    evtPtr_preClick(ptNorm);
};

void ptrEvtListener_internal::evtTouchRaw_up(int32_t ptrNum, int32_t x, int32_t y) {
    if (!validFirstDown)
        return;
    const ::glm::vec2 ptNorm = normalizeMouse * glm::vec3(x, y, 1.0f);
    if (withinClickRadius(ptNorm))
        evtPtr_confirmClick(ptNorm);
    else
        evtPtr_cancelClick();
    validFirstDown = false;
};

void ptrEvtListener_internal::evtTouchRaw_move(int32_t ptrNum, int32_t x, int32_t y) {
    if (!validFirstDown)
        return;

    const ::glm::vec2 ptNorm = normalizeMouse * glm::vec3(x, y, 1.0f);
    if (!withinClickRadius(ptNorm)) {
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

::glm::vec2 ptrEvtListener_internal::getLastMouseNormalized() {
    return normalizeMouse * glm::vec3(lastMouseX, lastMouseY, 1.0f);
}
}  // namespace MNOGLA