#include "../ptrEvtListener.h"
bool ptrEvtListener::feedEvtPtr(size_t n, int32_t* buf) {
    return rawMouseEvtListener::feedEvtMouse(n, buf) || rawTouchEvtListener::feedEvtTouch(n, buf) || false;
}

void ptrEvtListener_internal::evtTouchRaw_down(int32_t ptrNum, int32_t x, int32_t y) {
     evtPtr_preClick(x, y);
};

void ptrEvtListener_internal::evtMouseRaw_down(int32_t bnum) {
     if (bnum == 0)
        evtTouchRaw_down(/*ptrNum*/ -1, lastMouseX, lastMouseY);
}

void ptrEvtListener_internal::evtMouseRaw_up(int32_t bnum) {
    if (bnum == 0)
        evtTouchRaw_up(/*ptrNum*/ -1, lastMouseX, lastMouseY);
}
