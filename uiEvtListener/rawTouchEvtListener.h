#pragma once
#include "src/rawTouchEvtListener_internal.h"

// consumes mouse-related events and triggers callback functions, to be overridden by the implementation.
class rawTouchEvtListener : protected rawTouchEvtListener_internal {
   public:
    // offer event. Returns true if touch-related.
    virtual bool feedEvtTouch(size_t n, int32_t* buf);

    // called on touchscreen down
    virtual void evtTouchRaw_down(int32_t ptrNum, int32_t x, int32_t y) {}

    // called on touchscreen up
    virtual void evtTouchRaw_up(int32_t ptrNum, int32_t x, int32_t y) {}

    // called on touchscreen move
    virtual void evtTouchRaw_move(int32_t ptrNum, int32_t x, int32_t y) {}
    virtual ~rawTouchEvtListener() = default;
};
