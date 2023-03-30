#include <functional>
#include <glm/gtx/vector_angle.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "../MNOGLA.h"
#include "../twoD/twoDMatrix.h"
#include "../twoD/twoDView.h"
#include "../uiEvtListener/ptrEvtListener.h"
#include "guiElem.hpp"
#include "internal/guiContainerInternal.h"
#include "internal/rezoomer.hpp"

namespace MNOGLA {
using ::glm::vec2, ::glm::vec3, ::glm::mat3;
using ::std::shared_ptr, ::std::make_shared, ::std::string, ::std::vector, ::std::runtime_error;

// graphical top-level element that holds child elements e.g. buttons. Manages pan, zoom, rotation receiving ptrEvt input.
class guiContainer : public guiContainerInternal {
   public:
    guiContainer();

    void addElem(shared_ptr<guiElem> e);
    void unfreeze();
    void freeze();
    void autoscale();
    void render();

    virtual bool evtPtr_preClick(const vec2& ptNorm) override;
    virtual void evtPtr_secondary(const vec2& ptNorm) override;
    virtual void evtPtr_confirmClick(const vec2& ptNorm) override;
    virtual void evtPtr_cancelClick() override;
    virtual void evtPtr_drag(const vec2& deltaNorm) override;
    virtual void evtMouseRaw_wheel(int32_t deltaX, int32_t deltaY) override;
    virtual void evtPtr_twoPtrDrag(const vec2& pt1start_NDC, const vec2& pt1stop_NDC,
                                   const vec2& pt2start_NDC, const vec2& pt2stop_NDC) override;
    virtual void evtPtr_dragPanZoomEnds() override;
};
}  // namespace MNOGLA