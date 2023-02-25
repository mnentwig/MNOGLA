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
    void evtTouchRaw_up(int32_t ptrNum, int32_t nRemainingPointers) final;
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
    virtual void evtPtr_twoPtrDrag(const ::glm::vec2& pt1start, const ::glm::vec2& pt1stop,
                                   const ::glm::vec2& pt2start, const ::glm::vec2& pt2stop){};  // implementation overrides
   protected:
    ::glm::vec2 getLastMouseNormalized() const;
    ::glm::vec2 normalizeRawMouse(const ::glm::ivec2& xy) const;
    ::glm::vec2 normalizeRawMouse(int32_t x, int32_t y) const;

    ptrEvtListenerConfig config;
    glm::mat3 normalizeMouse;
    float aspectRatio;

    class multitouchPtr;
    typedef std::shared_ptr<multitouchPtr> pMultitouchPtr_t;

    // multitouch pointers by arbitrary (host-assigned) index
    ::std::map<int32_t, pMultitouchPtr_t> pointers;

    class cClickAction;
    ::std::shared_ptr<cClickAction> clickAction;

    class cDrag1ptAction;
    ::std::shared_ptr<cDrag1ptAction> drag1ptAction;

    class cDrag2ptAction;
    ::std::shared_ptr<cDrag2ptAction> drag2ptAction;
};
}  // namespace MNOGLA