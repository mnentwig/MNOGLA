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
#include "guiButton.hpp"
#include "internal/guiContainerInternal.h"
#include "internal/rezoomer.hpp"

namespace MNOGLA {
using ::glm::vec2, ::glm::vec3, ::glm::mat3;
using ::std::shared_ptr, ::std::make_shared, ::std::string, ::std::vector, ::std::runtime_error;

// graphical top-level element that holds child elements e.g. buttons. Manages pan, zoom, rotation receiving ptrEvt input.
class guiContainer : public guiContainerInternal {
   public:
    guiContainer();
    void render();
    shared_ptr<guiButton> button(int32_t x, int32_t y, int w, int h, const string& text);

    void autoscale();

    bool evtPtr_preClick(const vec2& ptNorm);
    void evtPtr_secondary(const vec2& ptNorm);
    void evtPtr_confirmClick(const vec2& ptNorm);
    void evtPtr_cancelClick();
    void evtPtr_drag(const vec2& deltaNorm);
    void evtMouseRaw_scroll(int32_t deltaX, int32_t deltaY);
    void evtPtr_twoPtrDrag(const vec2& pt1start_NDC, const vec2& pt1stop_NDC,
                           const vec2& pt2start_NDC, const vec2& pt2stop_NDC);
    void evtPtr_dragPanZoomEnds();

    void unfreeze();
    void freeze();
};
}  // namespace MNOGLA