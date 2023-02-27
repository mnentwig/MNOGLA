#pragma once
#include <glm/gtx/vector_angle.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <stdexcept>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>

#include "../../MNOGLA.h"
#include "../../twoD/twoDMatrix.h"
#include "guiButton.hpp"

namespace MNOGLA {
using ::glm::vec2, ::glm::vec3, ::glm::mat3, ::std::vector, ::std::runtime_error, ::std::shared_ptr;

class rezoomer {
   public:
    rezoomer(const vector<float>& angleSnap_deg) : points_world(), phi(0.0f), fRezoom(1.0f), offset(0.0f, 0.0f), world2screenCopy(1.0f), angleSnap_rad(angleSnap_deg) {
        for (float& elem : angleSnap_rad)
            elem *= M_PI / 180.0f;
    }

    void clearPts() {
        points_world.clear();
    }
    void enterPt(const glm::vec2& pt_world) {
        points_world.push_back(pt_world);
    }
    void enterItem(const shared_ptr<guiButton> pItem) {
        vector<vec2> itemPts = pItem->getPts();
        points_world.insert(points_world.end(), itemPts.begin(), itemPts.end());
    }

    void analyze(const glm::vec2& lastRefPt, const glm::mat3& world2NDC) {
        // === set this->world2screenCopy ===
        // defines the target for transformations below
        world2screenCopy = world2NDC;
        this->lastRefPt = lastRefPt;
        glm::mat3 m1 = world2NDC;
        //  === rotate: set this->phi ===
        if (angleSnap_rad.size() > 0) {
            // determine rotation applied by world2NDC matrix

            float phi_rad = twoDMatrix::matrixRotAngle_rad(m1);
            float bestPhi_rad = ::std::numeric_limits<float>::infinity();
            for (int wrap = -1; wrap <= 1; ++wrap)
                for (size_t ix = 0; ix < angleSnap_rad.size(); ++ix) {
                    float angleSnapWrap = angleSnap_rad[ix] + (float)wrap * 2.0f * M_PI;
                    if (std::abs(phi_rad - angleSnapWrap) < std::abs(phi_rad - bestPhi_rad))
                        bestPhi_rad = angleSnapWrap;
                }
            phi = bestPhi_rad - phi_rad;
            m1 = m1 * /*C*/ twoDMatrix::translate(lastRefPt) * /*B1*/ twoDMatrix::rot(phi) * /*A*/ twoDMatrix::translate(-lastRefPt);
        } else
            phi = 0;

        // === scale: set this->fRezoom ===
        // rezooming factor eliminates excess slack in one dimension (in sum over both edges, needs centering after scaling).
        // Note: Evaluated on NDC data (after m1 is applied) but applied as B2 in world coordinates at lastRefPt
        slack s1(points_world, m1);
        fRezoom = s1.getZoomAdjustment();

        // === re-center: set this->offset ===
        // positive slack: content may move closer to the edge
        // Note: evaluated and applied in NDC coordinates after world2screen)
        m1 = world2NDC * /*C*/ twoDMatrix::translate(lastRefPt) * /*B2*/ twoDMatrix::scale(fRezoom) * /*B1*/ twoDMatrix::rot(phi) * /*A*/ twoDMatrix::translate(-lastRefPt);
        slack s2(points_world, m1);
        vec2 offset_NDC = vec2(
            s2.getOffsetAdjustment(s2.getSlackXMin(), s2.getSlackXMax()),
            s2.getOffsetAdjustment(s2.getSlackYMin(), s2.getSlackYMax()));
        offset = /*D*/ offset_NDC;
    }

    // returns interpolation (fInterp = 0..1) between initial and rezoom result transformation matrices
    glm::mat3 getResult(float fInterp) {
        assert(fInterp >= 0);
        assert(fInterp <= 1.001f);
        const float fadeIn = (-std::cos(fInterp * 3.141592f) + 1.0f) / 2.0f;
        const float fadeOut = 1.0f - fadeIn;
        float fRezoomInterpolated = fadeOut * 1.0f + fadeIn * fRezoom;
        vec2 offsetInterpolated = fadeOut * vec2(0.0f, 0.0f) + fadeIn * offset;
        // note: column vectors - transformation are applied A first, world2screenCopy 2nd last, D last
        glm::mat3 m = /*D*/ twoDMatrix::translate(offsetInterpolated) * world2screenCopy *
                      /*C*/ twoDMatrix::translate(lastRefPt) *
                      /*B2*/ twoDMatrix::scale(fRezoomInterpolated) *
                      /*B1*/ twoDMatrix::rot(phi) *
                      /*A*/ twoDMatrix::translate(-lastRefPt);
        return m;
    }

   protected:
    // points describing the content
    vector<vec2> points_world;
    // rotation, scaling around this point (mouse position, center of pinch)
    vec2 lastRefPt;
    // analyze result: step 1 = rotation angle
    float phi;
    // analyze result: step 2 = scale by this
    float fRezoom;
    // analyze result: step 3 = shift by this
    vec2 offset;
    // analyzed world2screen matrix
    glm::mat3 world2screenCopy;

    vector<float> angleSnap_rad;
    class slack {
       public:
        slack(const vector<glm::vec2> points_world, const glm::mat3& world2NDC) {
            for (const auto& pt : points_world) {
                glm::vec2 pt_NDC = world2NDC * glm::vec3(pt, 1.0f);
                xMin_NDC = ::std::min(xMin_NDC, pt_NDC.x);
                xMax_NDC = ::std::max(xMax_NDC, pt_NDC.x);
                yMin_NDC = ::std::min(yMin_NDC, pt_NDC.y);
                yMax_NDC = ::std::max(yMax_NDC, pt_NDC.y);
            }
        }
        float xMin_NDC = ::std::numeric_limits<float>::infinity();
        float xMax_NDC = -::std::numeric_limits<float>::infinity();
        float yMin_NDC = ::std::numeric_limits<float>::infinity();
        float yMax_NDC = -::std::numeric_limits<float>::infinity();
        float getSlackXMin() { return xMin_NDC + 1.0f; }
        float getSlackXMax() { return 1.0f - xMax_NDC; }
        float getSlackYMin() { return yMin_NDC + 1.0f; }
        float getSlackYMax() { return 1.0f - yMax_NDC; }
        float getZoomAdjustment() {
            float minSlackSum = std::min(getSlackXMin() + getSlackXMax(), getSlackYMin() + getSlackYMax());
            const float actualSize = 2.0f - minSlackSum;
            const float targetSize = 2.0f;
            const float scaleUp = targetSize / actualSize;
            return std::max(scaleUp, 1.0f);
        }
        float getOffsetAdjustment(float negEndSlack, float posEndSlack) {
            if (/*room to give at pos end */ (posEndSlack > 0) && /*content over edge at neg end */ (negEndSlack < 0))
                return std::min(/*how much we need*/ -negEndSlack, /*how much we may give*/ posEndSlack);  // move towards positive end (positive offset)
            if (/*room to give at neg end */ (negEndSlack > 0) && /*content over edge at pos end */ (posEndSlack < 0))
                return std::max(/*how much we need*/ posEndSlack, /*how much we may give*/ -negEndSlack);  // move towards negative end (negative offset)
            return 0.0f;
        }
    };
};
}  // namespace MNOGLA