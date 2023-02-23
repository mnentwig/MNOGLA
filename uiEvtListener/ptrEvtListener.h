#pragma once
#include <glm/vec2.hpp>

#include "src/ptrEvtListener_internal.h"
namespace MNOGLA {

class ptrEvtListener : protected ptrEvtListener_internal {
   public:
    ptrEvtListener();
    ptrEvtListener(ptrEvtListenerConfig& config);
    // tell the openGl viewport geometry to normalize mouse / touchscreen input
    void informViewport(float x, float y, float w, float h);

    // feed MNOGA events. True if implemented (app may choose to skip further processing)
    bool feedEvtPtr(size_t n, int32_t* buf);
    // ptr went down
    virtual void evtPtr_preClick(::glm::vec2 xy){};  // implementation overrides
    // ptr went up without dragging too far
    virtual void evtPtr_confirmClick(::glm::vec2 xy){};  // implementation overrides
    // ptr moved too far from down location
    virtual void evtPtr_cancelClick(){};  // implementation overrides
    // RMB or long press
    virtual void evtPtr_secondary(::glm::vec2 xy){};  // implementation overrides

   private:
    virtual bool feedEvtTouch(size_t n, int32_t* buf) final { return false; };  // "private" blocks access, we call superclass. Dummy override, never called.
    virtual bool feedEvtMouse(size_t n, int32_t* buf) final { return false; };  // "private" blocks access, we call superclass. Dummy override, never called.
};
}  // namespace MNOGLA
