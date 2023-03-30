#include <functional>
#include <glm/gtx/vector_angle.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../MNOGLA.h"
#include "../../twoD/twoDMatrix.h"
#include "../../twoD/twoDView.h"
#include "../../uiEvtListener/ptrEvtListener.h"
#include "../guiElem.hpp"
#include "rezoomer.hpp"

namespace MNOGLA {
using ::glm::vec2, ::glm::vec3, ::glm::mat3;
using ::std::shared_ptr, ::std::make_shared, ::std::string, ::std::vector, ::std::runtime_error;

// graphical top-level element that holds child elements e.g. buttons. Manages pan, zoom, rotation receiving ptrEvt input.
class guiContainerInternal : public ptrEvtListener {
   public:
    guiContainerInternal();

   protected:

      protected:
    class rescaleControl {
       public:
        rescaleControl() : active(false), startTime(std::numeric_limits<uint64_t>::max()), endTime(std::numeric_limits<uint64_t>::max()) {}

        // starts rescale transition over given time interval (nanosecond unit)
        void start(uint64_t animTime_ns) {
            active = true;
            startTime = MNOGLA::lastTimestamp_nanosecs;
            endTime = startTime + animTime_ns;
        }

        // returns true if rezoom is ongoing. If so, returns animation interpolation factor (between start and stop transforms)
        bool eval(float& interp) {
            if (!active) return false;

            uint64_t now = MNOGLA::lastTimestamp_nanosecs;
            if (now > endTime) {
                active = false;
                startTime = std::numeric_limits<uint64_t>::max();  // redundant
                endTime = std::numeric_limits<uint64_t>::max();    // redundant
                return false;
            }
            assert(endTime > startTime);
            uint64_t rescaleDuration = endTime - startTime;
            uint64_t rescaleCurrentRel = now - startTime;
            interp = (float)rescaleCurrentRel / (float)rescaleDuration;
            return true;
        }

       protected:
        bool active;
        uint64_t startTime;
        uint64_t endTime;
    };

    vector<shared_ptr<guiElem>> elems;
    twoDView view;
    // open state allows adding / editing content. Render in closed state.
    bool isOpen = true;
    rezoomer rez;
    rescaleControl rezCtrl;
    vec2 dragPanZoomLastRefPtW;
    
};
}  // namespace MNOGLA
