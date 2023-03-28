// Purpose of this file:
// pull in various source files per include ===
//  - keeps CMAKE input file simple
//  - enables additional optimizations over independent object files
#include <string>

#include "guiButton.hpp"
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

}  // namespace MNOGLA