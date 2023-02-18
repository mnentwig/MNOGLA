#pragma once
#include "src/ptrEvtListener_internal.h"
class ptrEvtListener : protected ptrEvtListener_internal {
   public:
    ptrEvtListener() {}
    bool feedEvtPtr(size_t n, int32_t* buf);
    // ptr went down
    virtual void evtPtr_preClick(int32_t x, int32_t y){};
    // ptr went up without dragging too far
    virtual void evtPtr_confirmClick(int32_t x, int32_t y){};
    // ptr moved too far from down location
    virtual void evtPtr_cancelClick(int32_t x, int32_t y){};
    // RMB or long press
    virtual void evtPtr_secondary(int32_t x, int32_t y){};

   private:
    virtual bool feedEvtTouch(size_t n, int32_t* buf) final { return false; };  // "private" blocks access. Never called.
    virtual bool feedEvtMouse(size_t n, int32_t* buf) final { return false; };  // "private" blocks access. Never called.
};