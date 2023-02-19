#include "../ptrEvtListener.h"
ptrEvtListener_internal::ptrEvtListener_internal() : validFirstDown(false), firstDownPtr(0), firstDownX(0), firstDownY(0), config() {}
ptrEvtListener::ptrEvtListener() {}

ptrEvtListener::ptrEvtListener(ptrEvtListenerConfig& config) {
    this->config = config;
}

bool ptrEvtListener::feedEvtPtr(size_t n, int32_t* buf) {
    return rawMouseEvtListener::feedEvtMouse(n, buf) || rawTouchEvtListener::feedEvtTouch(n, buf) || false;
}

// is point x, y within configuration-defined click radius around firstDownPt?
bool ptrEvtListener_internal::withinClickRadius(int32_t x, int32_t y) {
    const int clickRadiusSq = config.clickRadius_pixels * config.clickRadius_pixels;
    const int dxSq = (x - firstDownX) * (x - firstDownX);
    const int dySq = (y - firstDownY) * (y - firstDownY);
    return dxSq + dySq <= clickRadiusSq;
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
    firstDownPtr = ptrNum;
    firstDownX = x;
    firstDownY = y;
    validFirstDown = true;
    evtPtr_preClick(x, y);
};

void ptrEvtListener_internal::evtTouchRaw_up(int32_t ptrNum, int32_t x, int32_t y) {
    if (!validFirstDown)
        return;
    if (withinClickRadius(x, y))
        evtPtr_confirmClick(x, y);
    else
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
            evtPtr_secondary(lastMouseX, lastMouseY);
            break;
        default:
            break;
    }
}

void ptrEvtListener_internal::evtMouseRaw_up(int32_t bnum) {
    if (bnum == 0)
        evtTouchRaw_up(/*ptrNum*/ -1, lastMouseX, lastMouseY);
}

void ptrEvtListener_internal::evtMouseRaw_move(int32_t x, int32_t y) {
    rawMouseEvtListener::evtMouseRaw_move(x, y);
    evtTouchRaw_move(/*ptrNum*/ -1, lastMouseX, lastMouseY);  // fake touchscreen pointer "-1"
}
