#pragma once
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "pattern.hpp"
#include "synth.hpp"
using std::string, std::to_string, std::runtime_error, std::vector, std::shared_ptr, std::make_shared, std::map;
#pragma once
namespace MNOGLA {
class midisynth {
   public:
    midisynth(){};
    void runAudio(float* dest, size_t nFrames, float audiotime_s) {
        for (auto it : synthByNote)
            it.second->run(dest, nFrames, audiotime_s);
    }
    void midiCb(int32_t v0, int32_t v1, int32_t v2) {}

   protected:
    map<size_t, shared_ptr<MNOGLA::mono1>> synthByNote;
};
}  // namespace MNOGLA