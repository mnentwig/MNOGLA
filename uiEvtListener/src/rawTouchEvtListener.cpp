#include "../rawTouchEvtListener.h"

#include <cassert>
namespace MNOGLA {
bool rawTouchEvtListener::feedEvtTouch(size_t n, int32_t* buf) {
    uint32_t key = buf[0];
    if (!n) return false;
    switch (key) {
        // note: we don't put code into our own implementation of the callback
        // so the user implementation is not required to call the superclass function.
        case MNOGLA::eKeyToHost::TOUCH_DOWN: {
            assert(n == 3 + 1);
            int32_t ptr = buf[1];
            int32_t x = buf[2];
            int32_t y = buf[3];
            evtTouchRaw_down(ptr, x, y);
            return true;
        }
        case MNOGLA::eKeyToHost::TOUCH_UP: {
            assert(n == 3 + 1);
            int32_t ptr = buf[1];
            int32_t x = buf[2];
            int32_t y = buf[3];
            evtTouchRaw_up(ptr, x, y);
            return true;
        }
        case MNOGLA::eKeyToHost::TOUCH_MOVE: {
            assert(n == 3 + 1);
            int32_t ptr = buf[1];
            int32_t x = buf[2];
            int32_t y = buf[3];
            evtTouchRaw_move(ptr, x, y);
            return true;
        }
        default:
            return false;
    }
}
}  // namespace MNOGLA