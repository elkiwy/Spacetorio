#ifndef IMGUI_CURVES_H_
#define IMGUI_CURVES_H_
// [src] https://github.com/ocornut/imgui/issues/123
// [src] https://github.com/ocornut/imgui/issues/55

// v1.23 - selection index track, value range, context menu, improve manipulation controls (D.Click to add/delete, drag to add)
// v1.22 - flip button; cosmetic fixes
// v1.21 - oops :)
// v1.20 - add iq's interpolation code
// v1.10 - easing and colors
// v1.00 - jari komppa's original

#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include <cmath>


namespace ImGui {
  int Curve(const char *label, const ImVec2 &size, const int maxpoints, ImVec2 *points, int *selection, const ImVec2 &rangeMin = ImVec2(0, 0), const ImVec2 &rangeMax = ImVec2(1, 1)) ;
  float CurveValue(float p, int maxpoints, const ImVec2 *points) ;
  float CurveValueSmooth(float p, int maxpoints, const ImVec2 *points) ;
}; // namespace ImGui




namespace tween {
  enum TYPE {
    LINEAR,

    QUADIN, // t^2
    QUADOUT,
    QUADINOUT,
    CUBICIN, // t^3
    CUBICOUT,
    CUBICINOUT,
    QUARTIN, // t^4
    QUARTOUT,
    QUARTINOUT,
    QUINTIN, // t^5
    QUINTOUT,
    QUINTINOUT,
    SINEIN, // sin(t)
    SINEOUT,
    SINEINOUT,
    EXPOIN, // 2^t
    EXPOOUT,
    EXPOINOUT,
    CIRCIN, // sqrt(1-t^2)
    CIRCOUT,
    CIRCINOUT,
    ELASTICIN, // exponentially decaying sine wave
    ELASTICOUT,
    ELASTICINOUT,
    BACKIN, // overshooting cubic easing: (s+1)*t^3 - s*t^2
    BACKOUT,
    BACKINOUT,
    BOUNCEIN, // exponentially decaying parabolic bounce
    BOUNCEOUT,
    BOUNCEINOUT,

    SINESQUARE,  // gapjumper's
    EXPONENTIAL, // gapjumper's
    SCHUBRING1,  // terry schubring's formula 1
    SCHUBRING2,  // terry schubring's formula 2
    SCHUBRING3,  // terry schubring's formula 3

    SINPI2, // tomas cepeda's
    SWING,  // tomas cepeda's & lquery's
  };


  static inline double ease(int easetype, double t) ;
} // namespace tween

namespace ImGui {
static const float CurveTerminator = -10000;

// [src] http://iquilezles.org/www/articles/minispline/minispline.htm
// key format (for dim == 1) is (t0,x0,t1,x1 ...)
// key format (for dim == 2) is (t0,x0,y0,t1,x1,y1 ...)
// key format (for dim == 3) is (t0,x0,y0,z0,t1,x1,y1,z1 ...)
template <int DIM> void spline(const float *key, int num, float t, float *v) ;

static inline float ImRemap(float v, float a, float b, float c, float d) ;

static inline ImVec2 ImRemap(const ImVec2 &v, const ImVec2 &a, const ImVec2 &b,
                             const ImVec2 &c, const ImVec2 &d) ;


}; // namespace ImGui

#endif // IMGUI_CURVES_H_
