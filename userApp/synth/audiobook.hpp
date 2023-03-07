#pragma once
#include <map>
#include <memory>
#include <stdexcept>

#include "../odsLoader/odsLoader.hpp"
#include "pattern.hpp"
#include "synth.hpp"

namespace MNOGLA {
using std::vector, std::cout, std::endl, std::string, std::map, std::runtime_error, std::shared_ptr, std::make_shared;
class audiobook {
   public:
    audiobook(const char* odsFilename, float audiorate_Hz) : patterns(), synths(), audiorate_Hz(audiorate_Hz) {
        MNOGLA::odsDoc l(odsFilename);

        auto blocks = l.getBlocks();
        for (const auto& b : blocks) {
            string id = b.getCellRel(/*leftmost*/ 0, /*topmost*/ 0, /*default*/ "");
            if (id == "$pattern") {
                pattern p(b);
                if (patterns.find(p.getName()) != patterns.end()) throw runtime_error("duplicate pattern: " + p.getName());
                patterns.insert({p.getName(), std::move(p)});
            }
        }
    }

   public:
    void run(float* dest, size_t nFrames) {
        while (nFrames) {
            size_t nFramesToLoop = std::ceil((audioloop_s - audiotime_s) * audiorate_Hz);
            nFramesToLoop = std::max(nFramesToLoop, (size_t)1);  // avoid getting stuck
            size_t nFramesNow = std::min(nFrames, nFramesToLoop);

            // run all synths
            //            abook->run(dest, nFramesNow, audiotime_s);
            audiotime_s += nFramesNow * 1.0f / audiorate_Hz;
            nFrames -= nFramesNow;
            if (audiotime_s > audioloop_s)
                audiotime_s -= audioloop_s;
        }
    }

   protected:
    map<string, pattern> patterns;
    vector<shared_ptr<MNOGLA::mono1>> synths;
    float audiorate_Hz;
    float audiotime_s = 0;
    float audioloop_s = 0;
};
}  // namespace MNOGLA