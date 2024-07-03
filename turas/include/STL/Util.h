#ifndef TURAS_STL_UTIL_H
#define TURAS_STL_UTIL_H
#include <utility>
#include <cmath>

namespace turas
{
    inline static float FAbs(float f) {
        return std::abs(f);
    }

    inline static float FMin(float a, float b) {
        return std::min(a, b);
    }
}

#endif