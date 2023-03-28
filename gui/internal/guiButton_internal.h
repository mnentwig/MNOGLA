#pragma once
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

#include "../../MNOGLA.h"
#include "../../twoD/twoDView.h"
#include "../guiElem.hpp"

namespace MNOGLA {
using ::glm::vec2, ::std::vector, ::std::string, ::glm::vec3;

class guiButton_internal : public guiElem {
   protected:
    guiButton_internal(int32_t x, int32_t y, int w, int h, const string& text) : x(x), y(y), w(w), h(h), preClickState(false), text(text), clickCb(nullptr) {}

   public:
    void render(MNOGLA::twoDView& v) override {
        const vec3& drawBgCol = preClickState ? bgColPreClick : bgCol;
        const vec3& drawTextCol = preClickState ? textColPreClick : textCol;
        v.filledRect(getTopLeft(), getBottomRight(), drawBgCol);
        v.vectorText(getTopLeft() + vec2(h / 4, 0), text, h, drawTextCol);
    }

    vector<vec2> getPts() const override {
        return vector<vec2>{vec2(x, y), vec2(x + w, y), vec2(x + w, y + h), vec2(x, y + h)};
    }

   protected:
    vec2 getTopLeft() const {
        return vec2(x, y);
    }
    vec2 getBottomRight() const {
        return vec2(x + w, y + h);
    }
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
    vec3 bgCol = vec3(0.2f, 0.2f, 0.2f);
    vec3 bgColPreClick = vec3(0.8f, 0.8f, 0.8f);
    vec3 textCol = vec3(0.8f, 0.8f, 0.8f);
    vec3 textColPreClick = vec3(0.2f, 0.2f, 0.2f);
};
}  // namespace MNOGLA