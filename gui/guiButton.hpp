#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <stdexcept>
#include <vector>

#include "../MNOGLA.h"
#include "../twoD/twoDView.h"
#include "internaL/guiButton_internal.h"
namespace MNOGLA {
using ::glm::vec2, ::glm::vec3, ::glm::mat3;
using ::std::shared_ptr, ::std::make_shared, ::std::string, ::std::vector, ::std::runtime_error;

class guiButton: public guiButton_internal {
   public:
    guiButton(int32_t x, int32_t y, int w, int h, const string& text) : guiButton_internal(x, y, w, h, text){}
    void setClickCallback(::std::function<void()> cb) { clickCb = cb; }
    void executeClickCallback() {
        if (clickCb != nullptr)
            clickCb();
    }

    bool evtPtr_preClick(const vec2& pt) override {
        bool hit = ptInside(pt);
        preClickState = hit;
        return hit;
    }
    void evtPtr_cancelClick() override {
        preClickState = false;
    }

    void evtPtr_confirmClick(const vec2& pt) override {
        if (!preClickState) return;
        executeClickCallback();
        preClickState = false;
    }

   protected:
};
}  // namespace MNOGLA