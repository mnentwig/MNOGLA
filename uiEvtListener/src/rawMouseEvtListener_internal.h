#pragma once
#include <map>

#include "../../MNOGLA.h"  // include for size_t, int32_t etc

// non-API content of rawMouseEvtListener
class rawMouseEvtListener_internal {
   protected:
    int32_t lastMouseX;
    int32_t lastMouseY;

    std::map<int32_t, bool> buttonState;
    void setButtonState(uint32_t bnum, bool state);
};