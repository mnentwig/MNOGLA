#include "../ptrEvtListener.h"

#include <cassert>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../../twoD/twoDMatrix.h"
namespace MNOGLA {
using ::glm::vec2, ::glm::vec3, ::glm::ivec2, ::std::shared_ptr, ::std::make_shared;

class ptrEvtListener_internal::multitouchPtr {
   public:
    multitouchPtr(const ivec2& initial)
        : initial(initial),
          current(initial),
          dragReported(initial),
          maxDistSquared(0.0f){};
    float getMaxDistSquared() const { return maxDistSquared; }

    // update pointer position to raw pixel coordinates xy
    void update(const ivec2& xy) {
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

class ptrEvtListener_internal::cDrag1ptAction {
   public:
    cDrag1ptAction(pMultitouchPtr_t ptr) : ptr(ptr) { assert(ptr); }
    pMultitouchPtr_t getPtr() const { return ptr; }

   protected:
    pMultitouchPtr_t ptr;
};

class ptrEvtListener_internal::cClickAction {
   public:
    cClickAction(pMultitouchPtr_t ptr) : ptr(ptr) { assert(ptr); }
    pMultitouchPtr_t getPtr() const { return ptr; }

   protected:
    pMultitouchPtr_t ptr;
};

class ptrEvtListener_internal::cDrag2ptAction {
   public:
    cDrag2ptAction(int32_t ixPtrA, pMultitouchPtr_t ptrA, int32_t ixPtrB, pMultitouchPtr_t ptrB) : ixPtrA(ixPtrA), ptrA(ptrA), ixPtrB(ixPtrB), ptrB(ptrB) {
        assert(ptrA);
        assert(ptrB);
        assert(ptrA != ptrB);
        assert(ixPtrA != ixPtrB);
    }
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
      clickAction(nullptr),
      drag1ptAction(nullptr),
      drag2ptAction(nullptr) {}
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
    //    logI("ptrEvtListener_internal::evtTouchRaw_down %d", ptrNum);

    // == get existing pointer - if any - before insertion ===
    // (which does not invalidate map iterators)
    auto itExistingPtr = pointers.begin();

    // === collect pointer ===
    pMultitouchPtr_t newPtr = make_shared<multitouchPtr>(ivec2(x, y));
    pointers.insert_or_assign(ptrNum, newPtr);
    size_t nPointersDown = pointers.size();

    // === process ongoing actions ===
    if (clickAction)
        evtPtr_cancelClick();   // 2nd pointer down invalidates a potential click
    if (drag2ptAction) return;  // there may be stray finger down events - ignore.

    // === clear ongoing actions ===
    clickAction = nullptr;
    drag1ptAction = nullptr;
    drag2ptAction = nullptr;

    // === decide next receiving action ===
    bool validClick = false;
    bool valid1ptDrag = false;
    bool valid2ptDrag = false;
    switch (nPointersDown) {
        case 1: {
            valid1ptDrag = true;

            const vec2 ptNorm = normalizeMouse * glm::vec3(x, y, 1.0f);
            if (evtPtr_preClick(ptNorm))  // ptr went down on something clickable
                validClick = true;
            break;
        }
        case 2:
            valid2ptDrag = true;
            break;
        default:
            break;
    }

    // === create new action ===
    if (validClick)
        clickAction = make_shared<cClickAction>(newPtr);
    if (valid1ptDrag)
        drag1ptAction = make_shared<cDrag1ptAction>(newPtr);
    if (valid2ptDrag) {
        drag2ptAction = make_shared<cDrag2ptAction>(ptrNum, newPtr, itExistingPtr->first, itExistingPtr->second);
    }
}

void ptrEvtListener_internal::evtTouchRaw_up(int32_t ptrNum, int32_t nRemainingPointers) {
    //    logI("ptrEvtListener_internal::evtTouchRaw_up %d", ptrNum);

    // === retrieve pointer ===
    auto itPtr = pointers.find(ptrNum);
    if (itPtr == pointers.end())
        return;  // guardrail

    // === pointer release ===
    if (clickAction) {
        pMultitouchPtr_t p = itPtr->second;
        if (clickAction->getPtr() == p) {
            ivec2 xy = p->getCurrent();
            const vec2 ptNorm = normalizeMouse * vec3((float)xy.x, (float)xy.y, 1.0f);
            evtPtr_confirmClick(ptNorm);
        } else {
            evtPtr_cancelClick();  // guardrail
        }
    }

    // === clear actions ===
    clickAction = nullptr;
    drag1ptAction = nullptr;
    drag2ptAction = nullptr;

    // === de-list pointer ===
    pointers.erase(itPtr);

    // === guardrail against OS/app inconsistency ===
    // note: negative numbers (used by mouse = "don't know") are ignored.
    if ((nRemainingPointers == 0) && (pointers.size() > 0)) {
        logI("!!! ptrEvtListener: Touch out of sync. Resetting all pointers !!!");
        pointers.clear();
    }
}

void ptrEvtListener_internal::evtTouchRaw_move(int32_t ptrNum, int32_t x, int32_t y) {
    //    logI("ptrEvtListener_internal::evtTouchRaw_move\t%d\t%d\t%d", ptrNum, x, y);

    // === retrieve pointer ===
    auto itPtr = pointers.find(ptrNum);
    if (itPtr == pointers.end())
        return;  // guardrail
    pMultitouchPtr_t p = itPtr->second;

    // === pointer position update ===
    p->update(ivec2(x, y));

    // === actions ===
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

    // drag1ptAction is fallthrough if clickAction is no longer possible
    if (drag1ptAction)
        if (drag1ptAction->getPtr() == p) {
            ivec2 drag = p->getDrag();
            const glm::vec2 ptNorm = normalizeRawMouse(drag.x, drag.y) - normalizeRawMouse(0, 0);  // hack to remove translation
            evtPtr_drag(ptNorm);
        }

    if (drag2ptAction)
        if (drag2ptAction->ptrTriggersAction(ptrNum)) {
            ivec2 pt1StartRaw, pt1StopRaw, pt2StartRaw, pt2StopRaw;
            drag2ptAction->get(pt1StartRaw, pt1StopRaw, pt2StartRaw, pt2StopRaw);
            vec2 pt1Start = normalizeRawMouse(pt1StartRaw);
            vec2 pt1Stop = normalizeRawMouse(pt1StopRaw);
            vec2 pt2Start = normalizeRawMouse(pt2StartRaw);
            vec2 pt2Stop = normalizeRawMouse(pt2StopRaw);
            // logI("raw\t%d\t%d\t%d\t%d", pt1StopRaw.x, pt1StopRaw.y, pt2StopRaw.x, pt2StopRaw.y);
            evtPtr_twoPtrDrag(pt1Start, pt1Stop, pt2Start, pt2Stop);
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
        // note: negative nRemainingPointers signals "not known" (don't mess up touchscreen state if used simultaneously)
        evtTouchRaw_up(/*ptrNum*/ -1, /*nRemainingPointers*/ -1);
    }
}

void ptrEvtListener_internal::evtMouseRaw_move(int32_t x, int32_t y) {
    rawMouseEvtListener::evtMouseRaw_move(x, y);              // update lastMouseX, lastMouseY
    evtTouchRaw_move(/*ptrNum*/ -1, lastMouseX, lastMouseY);  // fake touchscreen pointer "-1"
}

vec2 ptrEvtListener_internal::getLastMouseNormalized() const {
    return normalizeRawMouse(lastMouseX, lastMouseY);
}

vec2 ptrEvtListener_internal::normalizeRawMouse(int32_t x, int32_t y) const {
    return normalizeMouse * vec3(x, y, 1.0f);
}

vec2 ptrEvtListener_internal::normalizeRawMouse(const ivec2& xy) const {
    return normalizeMouse * vec3((float)xy.x, (float)xy.y, 1.0f);
}
}  // namespace MNOGLA