#ifndef TURAS_STL_ARRAY_H
#define TURAS_STL_ARRAY_H

#include <array>

namespace turas {
    template<typename T, size_t N>
    using Array = std::array<T, N>;
}

#endif