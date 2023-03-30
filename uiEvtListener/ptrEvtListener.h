#pragma once
#include <glm/vec2.hpp>

#include "src/ptrEvtListener_internal.h"
namespace MNOGLA {

// Purpose: Abstraction of mouse- and touchscreen UI for basic click-, pan-, zoom functionality.
class ptrEvtListener : protected ptrEvtListener_internal {
   public:
    ptrEvtListener();
    ptrEvtListener(ptrEvtListenerConfig& config);
    // tell the openGl viewport geometry to normalize mouse / touchscreen input
    void informViewport(float x, float y, float w, float h);

    // feed MNOGA events. True if implemented (app may choose to skip further processing)
    bool feedEvtPtr(size_t n, int32_t* buf);
    // ptr went down. Return true if it hit something clickable.
    virtual bool evtPtr_preClick(::glm::vec2 xy) { return false; };  // implementation overrides
    // ptr went up without dragging too far
    virtual void evtPtr_confirmClick(::glm::vec2 xy){};  // implementation overrides
    // ptr moved too far from down location
    void evtPtr_cancelClick() override{};  // implementation overrides
    // RMB or long press (TBD)
    virtual void evtPtr_secondary(::glm::vec2 xy){};                    // implementation overrides
    void evtPtr_drag(const ::glm::vec2& deltaNorm) override{};  // implementation overrides
    void evtPtr_twoPtrDrag(const ::glm::vec2& pt1start, const ::glm::vec2& pt1stop,
                                   const ::glm::vec2& pt2start, const ::glm::vec2& pt2stop) override{};  // implementation overrides
    void evtPtr_dragPanZoomEnds() override{};                                                    // implementation overrides

   private:
    bool feedEvtTouch(size_t n, int32_t* buf) final { return false; };  // "private" blocks access, we call superclass. Dummy override, never called.
    bool feedEvtMouse(size_t n, int32_t* buf) final { return false; };  // "private" blocks access, we call superclass. Dummy override, never called.
};
}  // namespace MNOGLA
