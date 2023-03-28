#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "../MNOGLA.h"
#include "../twoD/twoDView.h"
#include "internal/guiButton_internal.h"
namespace MNOGLA {
using ::glm::vec2, ::glm::vec3, ::glm::mat3;
using ::std::shared_ptr, ::std::make_shared, ::std::string, ::std::vector, ::std::runtime_error, std::string;

class guiButton : public guiButton_internal {
   public:
    guiButton(int32_t x, int32_t y, int w, int h, const string& text);
    void setClickCallback(::std::function<void()> cb);
    void executeClickCallback() override { guiButton_internal::executeClickCallback(); } // make internal function visible in API
};
}  // namespace MNOGLA