#pragma once
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../MNOGLA.h"
#include "../../twoD/twoDView.h"

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

    bool evtPtr_preClick(const vec2& pt) {
        bool hit = ptInside(pt);
        preClickState = hit;
        return hit;
    }
    void evtPtr_cancelClick() {
        preClickState = false;
    }

    void evtPtr_confirmClick(const vec2& pt) {
        if (!preClickState) return;
        executeClickCallback();
        preClickState = false;
    }

   protected:
    bool ptInside(vec2(pt)) {
        return (pt.x < x)        ? false
               : (pt.y < y)      ? false
               : (pt.x >= x + w) ? false
               : (pt.y >= y + h) ? false
                                 : true;
    }

    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    bool preClickState;
    string text;
    ::std::function<void()> clickCb;
};
}  // namespace MNOGLA