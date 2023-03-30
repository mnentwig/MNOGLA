#include "../rawMouseEvtListener.h"

#include <cassert>

namespace MNOGLA {
rawMouseEvtListener_internal::rawMouseEvtListener_internal() : lastMouseX(0), lastMouseY(0), buttonState() {}

bool rawMouseEvtListener::feedEvtMouse(size_t n, int32_t* buf) {
    uint32_t key = buf[0];
    if (!n) return false;
    switch (key) {
        // note: we don't put code e.g. "setButtonState()" into our own implementation of the callback
        // so the user implementation is not required to call the superclass function.
        case MNOGLA::eKeyToHost::MOUSE_DOWN: {
            assert(n == 1 + 1 /*nArgs + key*/);
            int32_t bnum = buf[1];
            setButtonState(bnum, true);
            evtMouseRaw_down(bnum);
            return true;
        }
        case MNOGLA::eKeyToHost::MOUSE_UP: {
            assert(n == 1 + 1 /*nArgs + key*/);
            int32_t bnum = buf[1];
            setButtonState(bnum, false);
            evtMouseRaw_up(bnum);
            return true;
        }
        case MNOGLA::eKeyToHost::MOUSE_MOVE: {
            assert(n == 2 + 1 /*nArgs + key*/);
            int32_t x = buf[1];
            int32_t y = buf[2];

            lastMouseX = x;
            lastMouseY = y;
            evtMouseRaw_move(x, y);
            return true;
        }
        case MNOGLA::eKeyToHost::MOUSE_WHEEL: {
            assert(n == 2 + 1 /*nArgs + key*/);
            int32_t deltaX = buf[1];
            int32_t deltaY = buf[2];
            evtMouseRaw_wheel(deltaX, deltaY);
            return true;
        }
        default:
            return false;
    }
}

void rawMouseEvtListener::getMouse(int32_t* mX, int32_t* mY) {
    *mX = lastMouseX;
    *mY = lastMouseX;
}

void rawMouseEvtListener_internal::setButtonState(uint32_t bnum, bool state) {
    buttonState[bnum] = state;
}

bool rawMouseEvtListener::getButtonState(uint32_t bnum) {
    return buttonState[bnum];
}
}  // namespace MNOGLA