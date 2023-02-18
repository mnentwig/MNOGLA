#include "../rawMouseEvtListener.h"
#include "../rawTouchEvtListener.h"
#pragma once
class ptrEvtListener_internal : public rawMouseEvtListener, public rawTouchEvtListener {
   public:
    ptrEvtListener_internal(){};
    void evtTouchRaw_down(int32_t ptrNum, int32_t x, int32_t y) final;
    void evtTouchRaw_up(int32_t ptrNum, int32_t x, int32_t y) final{}; // TBD
    void evtTouchRaw_move(int32_t ptrNum, int32_t x, int32_t y) final{}; // TBD
    void evtMouseRaw_down(int32_t bnum) final;
    void evtMouseRaw_up(int32_t bnum) final;
    void evtMouseRaw_move(int32_t x, int32_t y) final{}; // TBD

    virtual void evtPtr_preClick(int32_t x, int32_t y){}; // implementation overrides
};