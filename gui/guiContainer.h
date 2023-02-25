#include <functional>
#include <glm/gtx/vector_angle.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>

#include "../MNOGLA.h"
#include "../twoD/twoDMatrix.h"
#include "../twoD/twoDView.h"
#include "../uiEvtListener/ptrEvtListener.h"

namespace MNOGLA {
using ::glm::vec2, ::glm::vec3, ::glm::mat3;
using ::std::shared_ptr, ::std::make_shared, ::std::string, ::std::vector;
class guiButton {
   public:
    guiButton(int32_t x, int32_t y, int w, int h, const string& text) : x(x), y(y), w(w), h(h), preClickState(false), text(text), clickCb(nullptr) {}
    void setClickCallback(::std::function<void()> cb) { clickCb = cb; }
    void executeClickCallback() {
        if (clickCb != nullptr)
            clickCb();
    }
    vec2 getTopLeft() const {
        return vec2(x, y);
    }
    vec2 getBottomRight() const {
        return vec2(x + w, y + h);
    }
    void render(MNOGLA::twoDView& v) {
        const vec3& drawBgCol = preClickState ? bgColPreClick : bgCol;
        const vec3& drawTextCol = preClickState ? textColPreClick : textCol;
        v.filledRect(getTopLeft(), getBottomRight(), drawBgCol);
        v.vectorText(getTopLeft() + vec2(h / 4, 0), text, h, drawTextCol);
    }

    vec3 bgCol = vec3(0.2f, 0.2f, 0.2f);
    vec3 bgColPreClick = vec3(0.8f, 0.8f, 0.8f);
    vec3 textCol = vec3(0.8f, 0.8f, 0.8f);
    vec3 textColPreClick = vec3(0.2f, 0.2f, 0.2f);
    void setPreClickState(bool state) { preClickState = state; }
    bool getPreClickState() { return preClickState; }
    bool ptInside(vec2(pt)) {
        return (pt.x < x)        ? false
               : (pt.y < y)      ? false
               : (pt.x >= x + w) ? false
               : (pt.y >= y + h) ? false
                                 : true;
    }

   protected:
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    bool preClickState;
    string text;
    ::std::function<void()> clickCb;
};

class guiContainer : public ptrEvtListener {
   public:
    guiContainer() : buttons(), view(), panDownPt(0.0f, 0.0f), panDown(false) {}
    void render() {
        for (auto b : buttons)
            b->render(view);
    }

    shared_ptr<guiButton> button(int32_t x, int32_t y, int w, int h, const string& text) {
        buttons.push_back(::std::make_shared<guiButton>(x, y, w, h, text));
        return buttons.back();
    }

    void autoscale() {
        struct {
            void enterPt(const vec2& pt) {
                minPt = vec2(::std::min(minPt.x, pt.x), ::std::min(minPt.y, pt.y));
                maxPt = vec2(::std::max(maxPt.x, pt.x), ::std::max(maxPt.y, pt.y));
            }
            vec2 getCenter() { return (minPt + maxPt) / 2.0f; }
            vec2 getWh() { return (maxPt - minPt); }

           protected:
            vec2 minPt = vec2(::std::numeric_limits<float>::infinity(), ::std::numeric_limits<float>::infinity());
            vec2 maxPt = vec2(-::std::numeric_limits<float>::infinity(), -::std::numeric_limits<float>::infinity());

        } autoscaler;
        for (auto b : buttons) {
            autoscaler.enterPt(b->getTopLeft());
            autoscaler.enterPt(b->getBottomRight());
        }

        const vec2 wh = autoscaler.getWh();
        const vec2 viewWh(wh.x, wh.x / aspectRatio);
        view.set(autoscaler.getCenter(), viewWh, 0.0f);
    }

    bool evtPtr_preClick(const vec2& ptNorm) {
        MNOGLA::logI("pre-click %f %f", ptNorm.x, ptNorm.y);
        vec2 ptWorld = view.getScreen2world() * vec3(ptNorm, 1.0f);
        bool anyHit = false;
        for (auto b : buttons) {
            bool hit = b->ptInside(ptWorld);
            b->setPreClickState(hit);
            anyHit |= hit;
        }
        panDownPt = ptNorm;
        panDown = true;
        return anyHit;
    }

    void evtPtr_secondary(const vec2& ptNorm) {
        MNOGLA::logI("secondary click %f %f", ptNorm.x, ptNorm.y);
        for (auto b : buttons)
            b->setPreClickState(false);
    };

    void evtPtr_confirmClick(const vec2& ptNorm) {
        MNOGLA::logI("confirm click %f %f", ptNorm.x, ptNorm.y);
        for (auto b : buttons)
            if (b->getPreClickState()) {
                b->setPreClickState(false);
                b->executeClickCallback();
            }
        panDown = false;
    };

    void evtPtr_cancelClick() {
        for (auto b : buttons)
            b->setPreClickState(false);
        panDown = false;
    };

    void evtPtr_drag(const vec2& deltaNorm) {
        vec2 mouseDelta = view.getScreen2world() * vec3(deltaNorm, 1.0f) - view.getScreen2world() * vec3(0.0f, 0.0f, 1.0f);  // fixme
        // MNOGLA::logI("evtPtr drag %f %f", mouseDelta.x, mouseDelta.y);
        mat3 m = view.getWorld2screen();
        m = m * twoDMatrix::translate(mouseDelta);
        view.setWorld2screen(m);
    }

    void evtMouseRaw_scroll(int32_t deltaX, int32_t deltaY) {
        vec2 mousePos = view.getScreen2world() * vec3(getLastMouseNormalized(), 1.0f);
        mat3 m = view.getWorld2screen();
        m = m * twoDMatrix::translate(mousePos);
        m = m * twoDMatrix::rot((deltaX + deltaY) * 15.0f * 3.1415926 / 180.0f);
        m = m * twoDMatrix::translate(-mousePos);
        view.setWorld2screen(m);
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
    }

   protected:
    vector<shared_ptr<guiButton>> buttons;
    MNOGLA::twoDView view;

    // === pan controls ===
    vec2 panDownPt;
    bool panDown;
};
}  // namespace MNOGLA