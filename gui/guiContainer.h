#include <functional>
#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>

#include "../MNOGLA.h"
#include "../twoD/twoDView.h"
#include "../uiEvtListener/ptrEvtListener.h"

namespace MNOGLA {
using std::shared_ptr, std::make_shared, std::string, std::vector;
class guiButton {
   public:
    guiButton(int32_t x, int32_t y, int w, int h, const string& text) : x(x), y(y), w(w), h(h), preClickState(false), text(text), clickCb(nullptr) {}
    void setClickCallback(std::function<void()> cb) { clickCb = cb; }
    void render(MNOGLA::twoDView& v) {
        const glm::vec3& drawBgCol = preClickState ? bgColPreClick : bgCol;
        const glm::vec3& drawTextCol = preClickState ? textColPreClick : textCol;
        v.filledRect(glm::vec2(x, y), glm::vec2(x + w, y + h), drawBgCol);
        v.vectorText(glm::vec2(x, y), text, h, drawTextCol);
    }

    glm::vec3 bgCol = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 bgColPreClick = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 textCol = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 textColPreClick = glm::vec3(0.2f, 0.2f, 0.2f);
    void setPreClickState(bool state) { preClickState = state; }
    bool getPreClickState() { return preClickState; }
    bool ptInside(int32_t ptX, int32_t ptY) {
        return (ptX < x)        ? false
               : (ptY < y)      ? false
               : (ptX >= x + w) ? false
               : (ptY >= y + h) ? false
                                : true;
    }

   protected:
    int32_t x, y, w, h;
    bool preClickState;
    string text;
    std::function<void()> clickCb;
};

class guiContainer : public ptrEvtListener {
   public:
    guiContainer() : buttons() {}
    void render(MNOGLA::twoDView& v) {
        for (auto b : buttons)
            b->render(v);
    }

    std::shared_ptr<guiButton> button(int32_t x, int32_t y, int w, int h, const string& text) {
        buttons.push_back(std::make_shared<guiButton>(x, y, w, h, text));
        return buttons.back();
    }

    void evtPtr_preClick(int32_t x, int32_t y) {
        MNOGLA::logI("pre-click %d %d", x, y);
        for (auto b : buttons)
            b->setPreClickState(b->ptInside(x, y));
    }

    void evtPtr_secondary(int32_t x, int32_t y) {
        MNOGLA::logI("secondary click %d %d", x, y);
        for (auto b : buttons)
            b->setPreClickState(false);
    };

    void evtPtr_confirmClick(int32_t x, int32_t y) {
        MNOGLA::logI("confirm click %d %d", x, y);
        for (auto b : buttons)
            if (b->getPreClickState()) {
                b->setPreClickState(false);
                MNOGLA::logI("clicked!");
            }
    };

    void evtPtr_cancelClick() {
        for (auto b : buttons)
            b->setPreClickState(false);
    };

   protected:
    vector<shared_ptr<guiButton>> buttons;
};
}  // namespace MNOGLA