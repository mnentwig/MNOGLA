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
#include "internal/guiButton.hpp"
#include "internal/rezoomer.hpp"

namespace MNOGLA {
using ::glm::vec2, ::glm::vec3, ::glm::mat3;
using ::std::shared_ptr, ::std::make_shared, ::std::string, ::std::vector, ::std::runtime_error;

// graphical top-level element that holds child elements e.g. buttons. Manages pan, zoom, rotation receiving ptrEvt input.
class guiContainer : public ptrEvtListener {
   public:
    guiContainer() : buttons(), view(), rez(vector<float>{0, 45, 90, 135, 180, 225, 270, 315}), rezCtrl(), dragPanZoomLastRefPtW(0.0f, 0.0f) {}
    void render() {
        if (isOpen) throw runtime_error("guiContainer render() in open state. Must close() first");

        // === run rescale animation ===
        float rescaleInterp;
        if (rezCtrl.eval(rescaleInterp))
            view.setWorld2screen(rez.getResult(rescaleInterp));

        for (auto b : buttons)
            b->render(view);
    }

    shared_ptr<guiButton> button(int32_t x, int32_t y, int w, int h, const string& text) {
        if (!isOpen) throw runtime_error("guiContainer child creation in closed state. Must open() first");
        buttons.push_back(::std::make_shared<guiButton>(x, y, w, h, text));
        return buttons.back();
    }

    void autoscale() {
        struct {
            void enterPt(const vec2& pt) {
                minPt = vec2(::std::min(minPt.x, pt.x), ::std::min(minPt.y, pt.y));
                maxPt = vec2(::std::max(maxPt.x, pt.x), ::std::max(maxPt.y, pt.y));
            }
            void enterItem(const shared_ptr<guiButton> pItem) {
                enterPt(pItem->getTopLeft());
                enterPt(pItem->getBottomRight());
            }
            vec2 getCenter() { return (minPt + maxPt) / 2.0f; }
            vec2 getWh() { return (maxPt - minPt); }

           protected:
            vec2 minPt = vec2(::std::numeric_limits<float>::infinity(), ::std::numeric_limits<float>::infinity());
            vec2 maxPt = vec2(-::std::numeric_limits<float>::infinity(), -::std::numeric_limits<float>::infinity());

        } autoscaler;
        for (auto b : buttons)
            autoscaler.enterItem(b);

        const vec2 wh = autoscaler.getWh();
        const vec2 viewWh(wh.x, wh.x / aspectRatio);
        view.set(autoscaler.getCenter(), viewWh, 0.0f);
    }

    bool evtPtr_preClick(const vec2& ptNorm) {
        logI("pre-click %f %f", ptNorm.x, ptNorm.y);
        vec2 ptWorld = view.getScreen2world() * vec3(ptNorm, 1.0f);
        bool anyHit = false;
        for (auto b : buttons) {
            bool hit = b->ptInside(ptWorld);
            b->setPreClickState(hit);
            anyHit |= hit;
        }
        return anyHit;
    }

    void evtPtr_secondary(const vec2& ptNorm) {
        logI("secondary click %f %f", ptNorm.x, ptNorm.y);
        for (auto b : buttons)
            b->setPreClickState(false);
    };

    void evtPtr_confirmClick(const vec2& ptNorm) {
        logI("confirm click %f %f", ptNorm.x, ptNorm.y);
        for (auto b : buttons)
            if (b->getPreClickState()) {
                b->setPreClickState(false);
                b->executeClickCallback();
            }
    };

    void evtPtr_cancelClick() {
        for (auto b : buttons)
            b->setPreClickState(false);
    };

    void evtPtr_drag(const vec2& deltaNorm) {
        vec2 mouseDelta = view.getScreen2world() * vec3(deltaNorm, 1.0f) - view.getScreen2world() * vec3(0.0f, 0.0f, 1.0f);  // fixme
        // MNOGLA::logI("evtPtr drag %f %f", mouseDelta.x, mouseDelta.y);
        mat3 m = view.getWorld2screen();
        m = m * twoDMatrix::translate(mouseDelta);
        view.setWorld2screen(m);
        // no dragPanZoomLastRefPt but it shouldn't matter for translation only
    }

    void evtMouseRaw_scroll(int32_t deltaX, int32_t deltaY) {
        vec2 mousePosW = view.getScreen2world() * vec3(getLastMouseNormalized(), 1.0f);
        mat3 m = view.getWorld2screen();
        m = m * twoDMatrix::translate(mousePosW);
        m = m * twoDMatrix::rot((deltaX + deltaY) * 15.0f * 3.1415926 / 180.0f);
        m = m * twoDMatrix::translate(-mousePosW);
        view.setWorld2screen(m);
        dragPanZoomLastRefPtW = mousePosW;
    }

    void evtPtr_twoPtrDrag(const vec2& pt1start_NDC, const vec2& pt1stop_NDC,
                           const vec2& pt2start_NDC, const vec2& pt2stop_NDC) {
        vec2 p1startW = view.NDC2world(pt1start_NDC);
        vec2 p1stopW = view.NDC2world(pt1stop_NDC);
        vec2 p2startW = view.NDC2world(pt2start_NDC);
        vec2 p2stopW = view.NDC2world(pt2stop_NDC);

        // pinch vectors before and after event
        vec2 vStop = p2stopW - p1stopW;
        vec2 vStart = p2startW - p1startW;

        float vStopLen = glm::length(vStop);
        float vStartLen = glm::length(vStart);
        float phi_rad = glm::orientedAngle(vStart / vStartLen, vStop / vStopLen);
        vec2 centerStart = (p1startW + p2startW) / 2.0f;
        vec2 centerStop = (p1stopW + p2stopW) / 2.0f;

        mat3 m = view.getWorld2screen();
        m = m *
            twoDMatrix::translate(centerStop) *        // step 4: origin to center of stop line
            twoDMatrix::rot(-phi_rad) *                // step 3: rotate to new direction
            twoDMatrix::scale(vStopLen / vStartLen) *  // step 2: scale from startLen to stopLen
            twoDMatrix::translate(-centerStart);       // step 1: center of start line to origin
        view.setWorld2screen(m);
        dragPanZoomLastRefPtW = centerStop;
    }

    void evtPtr_dragPanZoomEnds() {
        // check whether view is in a suboptimal position (unused screen space)
        rez.analyze(dragPanZoomLastRefPtW, view.getWorld2screen());

        // prepare animation zooming towards optimized screen position (if no correction, this is a dummy animation)
        rezCtrl.start(/*nanoseconds*/ (uint64_t)(config.rescaleTime_ms * 1e6));
    };

    void unfreeze() {
        isOpen = true;
    }
    void freeze() {
        isOpen = false;
        rez.clearPts();
        for (const auto& b : buttons)
            rez.enterItem(b);
    }

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

    vector<shared_ptr<guiButton>> buttons;
    twoDView view;
    // open state allows adding / editing content. Render in closed state.
    bool isOpen = true;
    rezoomer rez;
    rescaleControl rezCtrl;
    vec2 dragPanZoomLastRefPtW;
};
}  // namespace MNOGLA