#include <glm/vec2.hpp>
#include <string>

#include "../gui/guiElem.hpp"

namespace MNOGLA {
using ::std::string, ::glm::vec3;
class multilineText : public guiElem {
   public:
    multilineText(const glm::vec2& topLeft, float fontsize, const string& text) : topLeft(topLeft), fontsize(fontsize) {

    }
    void render(MNOGLA::twoDView& v) override {
    }
    void setCol(const vec3& rgb) {
        this->rgb = rgb;
    }
    vector<vec2> getPts() const override {
        return vector<vec2>{topLeft, topLeft + vec2(100.0, 100.0)};  // TBD
    }

   protected:
    vec3 rgb;
    vec2 topLeft;
    float fontsize;
};
}  // namespace MNOGLA