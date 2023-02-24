#include "../ptrEvtListener.h"

#include <cassert>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../../twoD/twoDMatrix.h"
namespace MNOGLA {
using ::glm::vec2, ::glm::vec3, ::glm::ivec2, ::std::shared_ptr, ::std::make_shared;

class ptrEvtListener_internal::multitouchPtr {
   public:
    multitouchPtr(ivec2 initial)
        : initial(initial),
          current(initial),
          dragReported(initial),
          maxDistSquared(0.0f){};
    float getMaxDistSquared() const { return maxDistSquared; }

    // update pointer position to raw pixel coordinates xy
    void update(ivec2 xy) {
        int32_t dx = xy.x - initial.x;
        int32_t dy = xy.y - initial.y;
        maxDistSquared = std::max(maxDistSquared, (float)(dx * dx + dy * dy));
        current = xy;
    }

    // gets absolute raw pixel coordinates of last update
    const ivec2& getCurrent() const { return current; }

    // gets absolute raw pixel coordinates where ptr went down
    const ivec2& getInitial() const { return initial; }

    // gets delta (raw pixel coordinates) over last call to getDrag()
    ivec2 getDrag() {
        ivec2 r = current - dragReported;
        dragReported = current;
        return r;
    }

   protected:
    // raw pixel coordinates where ptr went down
    ivec2 initial;
    // raw pixel coordinates from last update
    ivec2 current;
    // raw pixel coordinates when getDrag() was last called
    ivec2 dragReported;
    // max. distance squared to initial over the lifetime
    float maxDistSquared;
};

class ptrEvtListener_internal::cClickAction {
   public:
    cClickAction(pMultitouchPtr_t ptr) : ptr(ptr) {}
    pMultitouchPtr_t getPtr() const { return ptr; }

   protected:
    shared_ptr<ptrEvtListener_internal::multitouchPtr> ptr;
};

class ptrEvtListener_internal::cTwoTouchAction {
   public:
    cTwoTouchAction(int32_t ixPtrA, pMultitouchPtr_t ptrA, int32_t ixPtrB, pMultitouchPtr_t ptrB) : ixPtrA(ixPtrA), ptrA(ptrA), ixPtrB(ixPtrB), ptrB(ptrB) {}
    bool ptrTriggersAction(int32_t ixPtr) const {
        return (ixPtr == ixPtrA) || (ixPtr == ixPtrB);
    }
    void get(ivec2& pt1Start, ivec2& pt1Stop, ivec2& pt2Start, ivec2& pt2Stop) {
        pt1Stop = ptrA->getCurrent();
        pt1Start = pt1Stop - ptrA->getDrag();
        pt2Stop = ptrB->getCurrent();
        pt2Start = pt2Stop - ptrB->getDrag();
    }

   protected:
    int32_t ixPtrA;
    pMultitouchPtr_t ptrA;
    int32_t ixPtrB;
    pMultitouchPtr_t ptrB;
};

ptrEvtListener_internal::ptrEvtListener_internal()
    : config(),
      normalizeMouse(/*identity matrix*/ 1.0f),
      aspectRatio(1.0f),
      pointers(),
      clickAction(nullptr), twoTouchAction(nullptr) {}
ptrEvtListener::ptrEvtListener() {}

ptrEvtListener::ptrEvtListener(ptrEvtListenerConfig& config) {
    this->config = config;
}

void ptrEvtListener::informViewport(float x, float y, float w, float h) {
    vec2 center(x + w / 2.0f, y + h / 2.0f);
    normalizeMouse = twoDMatrix::scale(vec2(2.0f / w, -2.0f / h)) * twoDMatrix::translate(-center);
    assert(h != 0);
    aspectRatio = w / h;  // conventional screensize-style definition e.g. 16:9
}

bool ptrEvtListener::feedEvtPtr(size_t n, int32_t* buf) {
    return rawMouseEvtListener::feedEvtMouse(n, buf) || rawTouchEvtListener::feedEvtTouch(n, buf) || false;
}

void ptrEvtListener_internal::evtTouchRaw_down(int32_t ptrNum, int32_t x, int32_t y) {
    const vec2 ptNorm = normalizeMouse * glm::vec3(x, y, 1.0f);

    pointers[ptrNum] = make_shared<multitouchPtr>(ivec2(x, y));

    // Note: we should be robust to recover from inconsistent host input as much as possible e.g. silently replace stale pointers and actions.
    bool validClick = false;
    if (pointers.size() == 1) {
        // first pointer down might become a click
        if (evtPtr_preClick(ptNorm)) {
            validClick = true;
        }
    }

    if (validClick) {
        // ptr went down on something clickable
        clickAction = make_shared<cClickAction>(pointers[ptrNum]);
    } else {
        if (clickAction) {
            // 2nd pointer down invalidates a potential click
            evtPtr_cancelClick();
            clickAction = nullptr;
        }
    }

    if (pointers.size() == 2) {
        // create two-finger touch action
        auto itA = pointers.begin();
        auto itB = ::std::next(itA);
        assert(::std::next(itB) == pointers.end());

        twoTouchAction = make_shared<cTwoTouchAction>(itA->first, itA->second, itB->first, itB->second);
    } else {
        twoTouchAction = nullptr;
    }
}

void ptrEvtListener_internal::evtTouchRaw_up(int32_t ptrNum, int32_t x, int32_t y) {
    // process the position update
    evtTouchRaw_move(ptrNum, x, y);
    if (clickAction) {
        if (clickAction->getPtr() == pointers[ptrNum]) {
            const vec2 ptNorm = normalizeMouse * vec3(x, y, 1.0f);
            evtPtr_confirmClick(ptNorm);
        } else {
            evtPtr_cancelClick();  // impossible
        }
        clickAction = nullptr;
    }

    execTwoTouchAction(ptrNum);
    twoTouchAction = nullptr;

    // stop tracking the pointer
    pointers.erase(ptrNum);
}

void ptrEvtListener_internal::evtTouchRaw_move(int32_t ptrNum, int32_t x, int32_t y) {
    pMultitouchPtr_t p = pointers[ptrNum];
    if (p.get() == nullptr) return;

    MNOGLA::logI("ptr %p", pointers[ptrNum].get());
    p->update(ivec2(x, y));
    if (clickAction) {
        if (p->getMaxDistSquared() <= config.clickRadius_pixels * config.clickRadius_pixels) {
            // treating as potential click - don't process as drag (yet)
            return;
        } else {
            evtPtr_cancelClick();
            clickAction = nullptr;
            // Note:
            // continue handling as drag from the down click position (not where the click got canceled)
            // this causes visual "snapping" but the drag distance matches the physical pointer movement.
        }
    }

    if (pointers.size() == 1) {
        ivec2 drag = p->getDrag();
        const glm::vec2 ptNorm = normalizeRawMouse(drag.x, drag.y) - normalizeRawMouse(0, 0);  // hack to remove translation
        evtPtr_drag(ptNorm);
    } else if (pointers.size() == 2) {
        execTwoTouchAction(ptrNum);
    }
}

void ptrEvtListener_internal::evtMouseRaw_down(int32_t bnum) {
    switch (bnum) {
        case 0:
            evtTouchRaw_down(/*ptrNum*/ -1, lastMouseX, lastMouseY);  // fake touchscreen pointer "-1"
            break;
        case 1:
            evtPtr_secondary(getLastMouseNormalized());
            break;
        default:
            break;
    }
}

void ptrEvtListener_internal::evtMouseRaw_up(int32_t bnum) {
    if (bnum == 0) {
        evtTouchRaw_up(/*ptrNum*/ -1, lastMouseX, lastMouseY);
    }
}

void ptrEvtListener_internal::evtMouseRaw_move(int32_t x, int32_t y) {
    rawMouseEvtListener::evtMouseRaw_move(x, y);              // update lastMouseX, lastMouseY
    evtTouchRaw_move(/*ptrNum*/ -1, lastMouseX, lastMouseY);  // fake touchscreen pointer "-1"
}

vec2 ptrEvtListener_internal::getLastMouseNormalized() {
    return normalizeRawMouse(lastMouseX, lastMouseY);
}

vec2 ptrEvtListener_internal::normalizeRawMouse(int32_t x, int32_t y) {
    return normalizeMouse * vec3(x, y, 1.0f);
}

vec2 ptrEvtListener_internal::normalizeRawMouse(const ivec2& xy) {
    return normalizeMouse * vec3((float)xy.x, (float)xy.y, 1.0f);
}

void ptrEvtListener_internal::execTwoTouchAction(int32_t ptrNum) {
    if (twoTouchAction.get() == nullptr)
        return;
    if (!twoTouchAction->ptrTriggersAction(ptrNum)) {
        ivec2 pt1StartRaw, pt1StopRaw, pt2StartRaw, pt2StopRaw;
        twoTouchAction->get(pt1StartRaw, pt1StopRaw, pt2StartRaw, pt2StopRaw);
        vec2 pt1Start = normalizeRawMouse(pt1StartRaw);
        vec2 pt1Stop = normalizeRawMouse(pt1Stop);
        vec2 pt2Start = normalizeRawMouse(pt2Start);
        vec2 pt2Stop = normalizeRawMouse(pt2Stop);
        evtPtr_twoPtrDrag(pt1Start, pt1Stop, pt2Start, pt2Stop);
    }
}
}  // namespace MNOGLA