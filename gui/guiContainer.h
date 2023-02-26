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

namespace MNOGLA {
using ::glm::vec2, ::glm::vec3, ::glm::mat3;
using ::std::shared_ptr, ::std::make_shared, ::std::string, ::std::vector, ::std::runtime_error;
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
    vector<vec2> getPts() const {
        return vector<vec2>{vec2(x, y), vec2(x + w, y), vec2(x + w, y + h), vec2(x, y + h)};
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

class rezoomer {
   public:
    rezoomer() : points_world(), fRezoom(1.0f), offset(0.0f, 0.0f), world2screenCopy(1.0f) {}

    void clearPts() {
        points_world.clear();
    }
    void enterPt(const glm::vec2& pt_world) {
        points_world.push_back(pt_world);
    }
    void enterItem(const shared_ptr<guiButton> pItem) {
        vector<vec2> itemPts = pItem->getPts();
        points_world.insert(points_world.end(), itemPts.begin(), itemPts.end());
    }

    void analyze(const glm::mat3& world2NDC) {
        // === set this->world2screenCopy ===
        // defines the target for transformations below
        world2screenCopy = world2NDC;

        // === scale: set this->fRezoom ===
        // rezooming factor eliminates excess slack in one dimension (in sum over both edges, needs centering after scaling).
        slack s1(points_world, world2NDC);
        fRezoom = s1.getZoomAdjustment();
        glm::mat3 m1 = twoDMatrix::scale(fRezoom) * world2NDC;

        // === re-center: set this->offset ===
        // positive slack: content may move closer to the edge
        slack s2(points_world, m1);
        vec2 offset_NDC = vec2(
            s2.getOffsetAdjustment(s2.getSlackXMin(), s2.getSlackXMax()),
            s2.getOffsetAdjustment(s2.getSlackYMin(), s2.getSlackYMax()));
        offset = glm::inverse(m1) * glm::vec3(offset_NDC, 1.0f) - glm::inverse(m1) * glm::vec3(0.0f, 0.0f, 1.0f);
        //        offset = glm::inverse(m1) * glm::vec3(offset_NDC, /*scale only. Don't use translation part of matrix*/ 0.0f);
        logI("rescaler analyze: scale=%f offset_NDC=%f, %f offset_world=%f, %f", fRezoom, offset_NDC.x, offset_NDC.y, offset.x, offset.y);
    }

    glm::mat3 getResult() {
        glm::mat3 m = world2screenCopy;
        m = m *
            twoDMatrix::scale(fRezoom) *    // step 2: apply rezooming factor
            twoDMatrix::translate(offset);  // step 1: apply offset (in world coordinates)
        return m;
    }

   protected:
    // points describing the content
    vector<vec2> points_world;
    // analyze result: step 1 = scale by this
    float fRezoom;
    // analyze result: step 2 = shift by this
    vec2 offset;
    // analyzed world2screen matrix
    glm::mat3 world2screenCopy;

    class slack {
       public:
        slack(const vector<glm::vec2> points_world, const glm::mat3& world2NDC) {
            for (const auto& pt : points_world) {
                glm::vec2 pt_NDC = world2NDC * glm::vec3(pt, 1.0f);
                xMin_NDC = ::std::min(xMin_NDC, pt_NDC.x);
                xMax_NDC = ::std::max(xMax_NDC, pt_NDC.x);
                yMin_NDC = ::std::min(yMin_NDC, pt_NDC.y);
                yMax_NDC = ::std::max(yMax_NDC, pt_NDC.y);
            }
        }
        float xMin_NDC = ::std::numeric_limits<float>::infinity();
        float xMax_NDC = -::std::numeric_limits<float>::infinity();
        float yMin_NDC = ::std::numeric_limits<float>::infinity();
        float yMax_NDC = -::std::numeric_limits<float>::infinity();
        float getSlackXMin() { return xMin_NDC + 1.0f; }
        float getSlackXMax() { return 1.0f - xMax_NDC; }
        float getSlackYMin() { return yMin_NDC + 1.0f; }
        float getSlackYMax() { return 1.0f - yMax_NDC; }
        float getZoomAdjustment() {
            float minSlackSum = std::min(getSlackXMin() + getSlackXMax(), getSlackYMin() + getSlackYMax());
            const float actualSize = 2.0f - minSlackSum;
            const float targetSize = 2.0f;
            const float scaleUp = targetSize / actualSize;
            return std::max(scaleUp, 1.0f);
        }
        float getOffsetAdjustment(float negEndSlack, float posEndSlack) {
            if (/*room to give at pos end */ (posEndSlack > 0) && /*content over edge at neg end */ (negEndSlack < 0))
                return std::min(/*how much we need*/ -negEndSlack, /*how much we may give*/ posEndSlack);  // move towards positive end (positive offset)
            if (/*room to give at neg end */ (negEndSlack > 0) && /*content over edge at pos end */ (posEndSlack < 0))
                return std::max(/*how much we need*/ posEndSlack, /*how much we may give*/ -negEndSlack);  // move towards negative end (negative offset)
            return 0.0f;
        }
    };
};

// graphical top-level element that holds child elements e.g. buttons. Manages pan, zoom, rotation receiving ptrEvt input.
class guiContainer : public ptrEvtListener {
   public:
    guiContainer() : buttons(), view(), rez() {}
    void render() {
        if (isOpen) throw runtime_error("guiContainer render() in open state. Must close() first");
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
        MNOGLA::logI("pre-click %f %f", ptNorm.x, ptNorm.y);
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

    void evtPtr_dragPanZoomEnds() {
        rez.analyze(view.getWorld2screen());
        view.setWorld2screen(rez.getResult());
    };

    void open() {
        isOpen = true;
    }
    void close() {
        isOpen = false;
        rez.clearPts();
        for (const auto& b : buttons)
            rez.enterItem(b);
    }

   protected:
    vector<shared_ptr<guiButton>> buttons;
    MNOGLA::twoDView view;
    // open state allows adding / editing content. Render in closed state.
    bool isOpen = true;
    rezoomer rez;
};
}  // namespace MNOGLA