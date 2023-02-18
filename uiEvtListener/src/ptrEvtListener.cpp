#include "../ptrEvtListener.h"
bool ptrEvtListener::feedEvtPtr(size_t n, int32_t* buf) {
    return rawMouseEvtListener::feedEvtMouse(n, buf) || rawTouchEvtListener::feedEvtTouch(n, buf) || false;
}

void ptrEvtListener_internal::evtTouchRaw_down(int32_t ptrNum, int32_t x, int32_t y) {
    if (validFirstDown) {
        validFirstDown = false;
        evtPtr_cancelClick();
        return;
    }
    firstDownPtr = ptrNum;
    firstDownX = x;
    firstDownY = y;
    validFirstDown = true;
    evtPtr_preClick(x, y);
};

void ptrEvtListener_internal::evtTouchRaw_up(int32_t ptrNum, int32_t x, int32_t y) {
    if (validFirstDown)
        evtPtr_confirmClick(x, y);
    validFirstDown = false;
};

void ptrEvtListener_internal::evtTouchRaw_move(int32_t ptrNum, int32_t x, int32_t y) {
    if (!validFirstDown)
        return;
    const int32_t maxDist = 50;
    if ((x - firstDownX) * (x - firstDownX) + (y - firstDownY) * (y - firstDownY) > maxDist * maxDist) {
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
    evtTouchRaw_move(/*ptrNum*/ -1, lastMouseX, lastMouseY); // fake touchscreen pointer "-1"
}
