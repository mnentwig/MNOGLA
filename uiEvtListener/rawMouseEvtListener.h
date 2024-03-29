#pragma once
#include "src/rawMouseEvtListener_internal.h"

namespace MNOGLA {
// consumes mouse-related events and triggers callback functions, to be overridden by the implementation.
class rawMouseEvtListener : protected rawMouseEvtListener_internal {
   public:
    // offer event. Returns true if mouse related.
    virtual bool feedEvtMouse(size_t n, int32_t* buf);

    // called on mouse button down (not touchscreen pointer!)
    virtual void evtMouseRaw_down(int32_t bnum) {}

    // called on mouse button up (not touchscreen pointer!)
    virtual void evtMouseRaw_up(int32_t bnum) {}

    // called on mouse movement (not touchscreen pointer!)
    virtual void evtMouseRaw_move(int32_t x, int32_t y) {}

    // called on mouse scrollwheel event
    virtual void evtMouseRaw_wheel(int32_t deltaX, int32_t deltaY) {}

    // returns mouse position into mX, mY
    void getMouse(int32_t* mX, int32_t* mY);

    // returns state of given button
    bool getButtonState(uint32_t bnum);

    // (destruct correctly for delete after upcasting)
    virtual ~rawMouseEvtListener() = default;
};
}  // namespace MNOGLA