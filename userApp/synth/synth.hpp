#pragma once
#include <stdint.h>

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../odsLoader/odsLoader.hpp"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
using std::vector, std::cout, std::endl, std::string;
class mono1 {
   public:
    mono1(float tStart_s, float tStep_s, const vector<int>& steps) : tStart_s(tStart_s), tStep_s(tStep_s), steps(steps) {}
    void run(float* dest, size_t nFrames, float time_s) {
        if (time_s < tStart_s) {
            // advance to start
            size_t framesToStart = (tStart_s - time_s) / tFrame_s;
            framesToStart = std::min(framesToStart, nFrames);
            if (framesToStart <= nFrames) return;  // don't reach start
            nFrames -= framesToStart;
            time_s = tStart_s;
            dest += framesToStart * interleave;
        }
        while (nFrames > 0) {
            size_t step = (time_s - tStart_s) / tStep_s;
            //            cout << time_s << "\t" << tStart_s << "\t" << tStep_s << "\t" << step << endl;
            if (step >= steps.size()) {
                dphi = 0;
                return;  // reached end
            }
            float remainingTimeInStep_s = tStart_s + (float)(step + 1) * tStep_s - time_s;
            size_t framesInStep = (int)(remainingTimeInStep_s / tFrame_s);
            size_t nRender = std::max(framesInStep, (size_t)1);  // avoid getting stuck
            nRender = std::min(nRender, nFrames);
            // trigger note
            if (step != lastTriggeredStep)
                triggerStep(step);
            // generate audio
            run(dest, interleave, nRender);
            nFrames -= nRender;
            time_s += (float)nRender * tStep_s;
            dest += nRender * interleave;
        }
    }
    float getTStart_s() {
        return tStart_s;
    }
    float getTStop_s() {
        return tStart_s + steps.size() * tStep_s;
    }
    static void
    config(size_t _interleave, float _tFrame_s) {
        interleave = _interleave;
        tFrame_s = _tFrame_s;
    }

   protected:
    void triggerStep(size_t step) {
        lastTriggeredStep = step;
        assert(step < steps.size());
        float f_Hz = std::pow(2.0f, (steps[step] - 69) / 12.0f) * 440.0f;
        dphi = f_Hz * tFrame_s * M_PI;
//        cout << "trig " << step << "\t" << f_Hz << "\t" << dphi << endl;
        amp = 10.0f;
        famp = 0.9999;
        float wrap = std::floor(phi / (2.0f * M_PI));
        phi -= wrap * 2.0f * M_PI;
    }
    void run(float* dest, size_t interleave, size_t nFrames) {
        for (size_t ix = 0; ix < nFrames; ++ix) {
            *dest = 0.1f * cos(amp * cos(phi));
            phi += dphi;
            dest += interleave;
            amp *= famp;
        }
    }

   public:
    void loadOds(const string& fname) {
        MNOGLA::odsDoc l(fname);
        const size_t nSheets = l.getNSheets();
        for (size_t ixSheet = 0; ixSheet < nSheets; ++ixSheet) {
            size_t nRows;
            size_t nCols;
            l.getSize(ixSheet, &nRows, &nCols);
            for (size_t ixRow = 0; ixRow < nRows; ++ixRow)
                for (size_t ixCol = 0; ixCol < nCols; ++ixCol) {
                    string val;
                    if (!l.getCell(ixSheet, ixRow, ixCol, /*out*/ val)) continue;
                    if ((val.size() < 1) || (val[0] != '$')) continue;
                    MNOGLA::odsDoc::block b(l, ixSheet, ixRow, ixCol);
                    cout << "got block" << endl;
                }
        }
    }

    float tStart_s;
    float tStep_s;
    float amp = 0;
    float famp = 0;
    vector<int> steps;
    static size_t interleave;
    static float tFrame_s;
    size_t lastTriggeredStep = /*any large number*/ 0xFFFFFFFF;
    float phi = 0;
    float dphi = 0;
};
float mono1::tFrame_s;
size_t mono1::interleave;