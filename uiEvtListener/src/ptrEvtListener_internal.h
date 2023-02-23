#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>

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

    virtual void evtPtr_preClick(const ::glm::vec2& ptNorm){};      // implementation overrides ("internal" version needed to call it from here)
    virtual void evtPtr_confirmClick(const ::glm::vec2& ptNorm){};  // implementation overrides ("internal" version needed to call it from here)
    virtual void evtPtr_cancelClick(){};                            // implementation overrides ("internal" version needed to call it from here)
    virtual void evtPtr_secondary(const ::glm::vec2& ptNorm){};     // implementation overrides ("internal" version needed to call it from here)
   protected:
    bool withinClickRadius(const ::glm::vec2& ptNorm) const;
    ::glm::vec2 getLastMouseNormalized();
    bool validFirstDown;
    int32_t firstDownPtr;
    ::glm::vec2 firstDownPt;
    ptrEvtListenerConfig config;
    glm::mat3 normalizeMouse;
};
}  // namespace MNOGLA