#include "../rawMouseEvtListener.h"
#include "../rawTouchEvtListener.h"
#pragma once
class ptrEvtListener_internal : public rawMouseEvtListener, public rawTouchEvtListener {
   public:
    ptrEvtListener_internal();
    void evtTouchRaw_down(int32_t ptrNum, int32_t x, int32_t y) final;
    void evtTouchRaw_up(int32_t ptrNum, int32_t x, int32_t y) final;
    void evtTouchRaw_move(int32_t ptrNum, int32_t x, int32_t y) final;
    void evtMouseRaw_down(int32_t bnum) final;
    void evtMouseRaw_up(int32_t bnum) final;
    void evtMouseRaw_move(int32_t x, int32_t y) final;

    virtual void evtPtr_preClick(int32_t x, int32_t y){};      // implementation overrides ("internal" version needed to call it from here)
    virtual void evtPtr_confirmClick(int32_t x, int32_t y){};  // implementation overrides ("internal" version needed to call it from here)
    virtual void evtPtr_cancelClick(){};                       // implementation overrides ("internal" version needed to call it from here)
    virtual void evtPtr_secondary(int32_t x, int32_t y){};     // implementation overrides ("internal" version needed to call it from here)
   protected:
    bool withinClickRadius(int32_t x, int32_t y);
    bool validFirstDown;
    int32_t firstDownPtr;
    int32_t firstDownX;
    int32_t firstDownY;
    ptrEvtListenerConfig config;
};