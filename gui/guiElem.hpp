
#pragma once
#include <glm/vec2.hpp>
#include <vector>

#include "../MNOGLA.h"
#include "../twoD/twoDView.h"
#include "internal/guiElem_internal.hpp"

namespace MNOGLA {
using ::glm::vec2, ::std::vector;

class guiElem : public guiElem_internal {
    public:
    virtual vector<vec2> getPts() const = 0;
    virtual bool evtPtr_preClick(const vec2& pt) = 0;
    virtual void evtPtr_cancelClick() = 0;
    virtual void evtPtr_confirmClick(const vec2& pt) = 0;
    virtual void render(MNOGLA::twoDView& v) = 0;
};
}  // namespace MNOGLA