// Purpose of this file:
// pull in various source files per include ===
//  - keeps CMAKE input file simple
//  - enables additional optimizations over independent object files
#include <string>

#include "guiButton.hpp"
#include "guiContainer.h"
// #include "internal/guiButton_internal.h"

using std::string;
namespace MNOGLA {
guiButton::guiButton(int32_t x, int32_t y, int w, int h, const string& text) : guiButton_internal(x, y, w, h, text) {}
void guiButton::setClickCallback(::std::function<void()> cb) { clickCb = cb; }
void guiButton_internal::executeClickCallback() {
    if (clickCb != nullptr)
        clickCb();
}
bool guiButton_internal::evtPtr_preClick(const vec2& pt) /*override*/ {
    bool hit = ptInside(pt);
    preClickState = hit;
    return hit;
}
void guiButton_internal::evtPtr_cancelClick() /*override*/ {
    preClickState = false;
}
void guiButton_internal::evtPtr_confirmClick(const vec2& pt) /*override*/ {
    if (!preClickState) return;
    executeClickCallback();
    preClickState = false;
}

// =========================================================
// guiContainer
// =========================================================

guiContainer::guiContainer() {}
void guiContainer::unfreeze() {
    isOpen = true;
}
void guiContainer::freeze() {
    isOpen = false;
    rez.clearPts();
    for (const auto& b : buttons)
        rez.enterItem(b);
}
void guiContainer::render() {
    if (isOpen) throw runtime_error("guiContainer render() in open state. Must close() first");

    // === run rescale animation ===
    float rescaleInterp;
    if (rezCtrl.eval(rescaleInterp))
        view.setWorld2screen(rez.getResult(rescaleInterp));

    for (auto b : buttons)
        b->render(view);
}
shared_ptr<guiButton> guiContainer::button(int32_t x, int32_t y, int w, int h, const string& text) {
    if (!isOpen) throw runtime_error("guiContainer child creation in closed state. Must open() first");
    shared_ptr<guiButton> b = ::std::make_shared<guiButton>(x, y, w, h, text);
    buttons.push_back(b);
    return b;
}

void guiContainer::autoscale() {
    struct {
        void enterPt(const vec2& pt) {
            minPt = vec2(::std::min(minPt.x, pt.x), ::std::min(minPt.y, pt.y));
            maxPt = vec2(::std::max(maxPt.x, pt.x), ::std::max(maxPt.y, pt.y));
        }
        void enterItem(const shared_ptr<guiElem> pElem) {
            const vector<vec2> pts = pElem->getPts();
            for (auto& p : pts)
                enterPt(p);
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
bool guiContainer::evtPtr_preClick(const vec2& ptNorm) {
    logI("pre-click %f %f", ptNorm.x, ptNorm.y);
    vec2 ptWorld = view.getScreen2world() * vec3(ptNorm, 1.0f);
    bool anyHit = false;
    for (auto b : buttons)
        anyHit |= b->evtPtr_preClick(ptWorld);
    return anyHit;
}

void guiContainer::evtPtr_secondary(const vec2& ptNorm) {
    logI("secondary click %f %f", ptNorm.x, ptNorm.y);
    for (auto b : buttons)
        b->evtPtr_cancelClick();
}

void guiContainer::evtPtr_confirmClick(const vec2& ptNorm) {
    logI("confirm click %f %f", ptNorm.x, ptNorm.y);
    vec2 ptWorld = view.getScreen2world() * vec3(ptNorm, 1.0f);
    for (auto b : buttons)
        b->evtPtr_confirmClick(ptWorld);
}

void guiContainer::evtPtr_cancelClick() {
    for (auto b : buttons)
        b->evtPtr_cancelClick();
}

void guiContainer::evtPtr_drag(const vec2& deltaNorm) {
    vec2 mouseDelta = view.getScreen2world() * vec3(deltaNorm, 1.0f) - view.getScreen2world() * vec3(0.0f, 0.0f, 1.0f);  // fixme
    // MNOGLA::logI("evtPtr drag %f %f", mouseDelta.x, mouseDelta.y);
    mat3 m = view.getWorld2screen();
    m = m * twoDMatrix::translate(mouseDelta);
    view.setWorld2screen(m);
    // no dragPanZoomLastRefPt but it shouldn't matter for translation only
}

void guiContainer::evtMouseRaw_scroll(int32_t deltaX, int32_t deltaY) {
    vec2 mousePosW = view.getScreen2world() * vec3(getLastMouseNormalized(), 1.0f);
    mat3 m = view.getWorld2screen();
    m = m * twoDMatrix::translate(mousePosW);
    m = m * twoDMatrix::rot((deltaX + deltaY) * 15.0f * 3.1415926 / 180.0f);
    m = m * twoDMatrix::translate(-mousePosW);
    view.setWorld2screen(m);
    dragPanZoomLastRefPtW = mousePosW;
}

void guiContainer::evtPtr_twoPtrDrag(const vec2& pt1start_NDC, const vec2& pt1stop_NDC,
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

void guiContainer::evtPtr_dragPanZoomEnds() {
    // check whether view is in a suboptimal position (unused screen space)
    rez.analyze(dragPanZoomLastRefPtW, view.getWorld2screen());

    // prepare animation zooming towards optimized screen position (if no correction, this is a dummy animation)
    rezCtrl.start(/*nanoseconds*/ (uint64_t)(config.rescaleTime_ms * 1e6));
};

}  // namespace MNOGLA