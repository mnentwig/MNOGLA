#include <functional>
#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>

#include "../MNOGLA.h"
#include "../twoD/twoDMatrix.h"
#include "../twoD/twoDView.h"
#include "../uiEvtListener/ptrEvtListener.h"

namespace MNOGLA {
using ::std::shared_ptr, ::std::make_shared, ::std::string, ::std::vector;
class guiButton {
   public:
    guiButton(int32_t x, int32_t y, int w, int h, const string& text) : x(x), y(y), w(w), h(h), preClickState(false), text(text), clickCb(nullptr) {}
    void setClickCallback(::std::function<void()> cb) { clickCb = cb; }
    void executeClickCallback() {
        if (clickCb != nullptr)
            clickCb();
    }
    glm::vec2 getTopLeft() const {
        return glm::vec2(x, y);
    }
    glm::vec2 getBottomRight() const {
        return glm::vec2(x + w, y + h);
    }
    void render(MNOGLA::twoDView& v) {
        const glm::vec3& drawBgCol = preClickState ? bgColPreClick : bgCol;
        const glm::vec3& drawTextCol = preClickState ? textColPreClick : textCol;
        v.filledRect(getTopLeft(), getBottomRight(), drawBgCol);
        v.vectorText(getTopLeft(), text, h, drawTextCol);
    }

    glm::vec3 bgCol = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 bgColPreClick = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 textCol = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 textColPreClick = glm::vec3(0.2f, 0.2f, 0.2f);
    void setPreClickState(bool state) { preClickState = state; }
    bool getPreClickState() { return preClickState; }
    bool ptInside(glm::vec2(pt)) {
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
            void enterPt(const glm::vec2& pt) {
                minPt = glm::vec2(::std::min(minPt.x, pt.x), ::std::min(minPt.y, pt.y));
                maxPt = glm::vec2(::std::max(maxPt.x, pt.x), ::std::max(maxPt.y, pt.y));
            }
            glm::vec2 getCenter() { return (minPt + maxPt) / 2.0f; }
            glm::vec2 getWh() { return (maxPt - minPt); }

           protected:
            glm::vec2 minPt = glm::vec2(::std::numeric_limits<float>::infinity(), ::std::numeric_limits<float>::infinity());
            glm::vec2 maxPt = glm::vec2(-::std::numeric_limits<float>::infinity(), -::std::numeric_limits<float>::infinity());

        } autoscaler;
        for (auto b : buttons) {
            autoscaler.enterPt(b->getTopLeft());
            autoscaler.enterPt(b->getBottomRight());
        }

        view.set(autoscaler.getCenter(), autoscaler.getWh(), 0.0f);
    }

    void evtPtr_preClick(const ::glm::vec2& ptNorm) {
        MNOGLA::logI("pre-click %f %f", ptNorm.x, ptNorm.y);
        for (auto b : buttons)
            b->setPreClickState(b->ptInside(ptNorm));
        panDownPt = ptNorm;
        panDown = true;
    }

    void evtPtr_secondary(const ::glm::vec2& ptNorm) {
        MNOGLA::logI("secondary click %f %f", ptNorm.x, ptNorm.y);
        for (auto b : buttons)
            b->setPreClickState(false);
    };

    void evtPtr_confirmClick(const ::glm::vec2& ptNorm) {
        MNOGLA::logI("confirm click %f %f", ptNorm.x, ptNorm.y);
        for (auto b : buttons)
            if (b->getPreClickState()) {
                b->setPreClickState(false);
                b->executeClickCallback();
                MNOGLA::logI("clicked!");
            }
        panDown = false;
    };

    void evtPtr_cancelClick() {
        for (auto b : buttons)
            b->setPreClickState(false);
        panDown = false;
    };

   protected:
    vector<shared_ptr<guiButton>> buttons;
    MNOGLA::twoDView view;

    // === pan controls ===
    glm::vec2 panDownPt;
    bool panDown;
};
}  // namespace MNOGLA