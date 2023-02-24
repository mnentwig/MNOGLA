#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>
#include <map>
#include <memory>

#include "../ptrEvtListenerConfig.h"
#include "../rawMouseEvtListener.h"
#include "../rawTouchEvtListener.h"
#pragma once
namespace MNOGLA {
class ptrEvtListener_internal : public rawMouseEvtListener, public rawTouchEvtListener {
   public:
    ptrEvtListener_internal();
    void evtTouchRaw_down(int32_t ptrNum, int32_t x, int32_t y) final;
    void evtTouchRaw_up(int32_t ptrNum, int32_t x, int32_t y) final;
    void evtTouchRaw_move(int32_t ptrNum, int32_t x, int32_t y) final;
    void evtMouseRaw_down(int32_t bnum) final;
    void evtMouseRaw_up(int32_t bnum) final;
    void evtMouseRaw_move(int32_t x, int32_t y) final;

    // return true if hitting something "clickable" (disables drag detection within config.clickRadius_pixels)
    virtual bool evtPtr_preClick(const ::glm::vec2& ptNorm) { return false; };  // implementation overrides ("internal" version needed to call it from here)
    virtual void evtPtr_confirmClick(const ::glm::vec2& ptNorm){};              // implementation overrides ("internal" version needed to call it from here)
    virtual void evtPtr_cancelClick(){};                                        // implementation overrides ("internal" version needed to call it from here)
    virtual void evtPtr_secondary(const ::glm::vec2& ptNorm){};                 // implementation overrides ("internal" version needed to call it from here)
    virtual void evtPtr_drag(const ::glm::vec2& deltaNorm){};                   // implementation overrides ("internal" version needed to call it from here)
   protected:
    ::glm::vec2 getLastMouseNormalized();
    ::glm::vec2 normalizeRawMouse(int32_t x, int32_t y);

    // whether firstDownPt(raw) qualifies for a click
    bool validFirstDown;
    // index of first multitouch pointer that became active (redundant - remove?)
    int32_t firstDownPtr;
    // click detection: point where mouse/touch went down; drag: current drag position (raw mouse coordinates in pixels)
    int32_t firstDownPtRawX;
    int32_t firstDownPtRawY;
    ptrEvtListenerConfig config;
    glm::mat3 normalizeMouse;
    float aspectRatio;

    class multitouchPtr;
    typedef std::shared_ptr<multitouchPtr> pMultitouchPtr_t;

    // multitouch pointers by arbitrary (host-assigned) index
    std::map<int32_t, pMultitouchPtr_t> pointers;

    class cClickAction;
    std::shared_ptr<cClickAction> clickAction;
};
}  // namespace MNOGLA