#ifndef TURAS_STL_TYPE_TRAITS_H
#define TURAS_STL_TYPE_TRAITS_H

#include <type_traits>

namespace turas {
    template<typename A, typename B>
    using IsBaseOf = std::is_base_of<A, B>;


    template<class T>
    constexpr T &&Forward(std::remove_reference_t<T> &t) noexcept {
        return std::forward<T>(t);
    }

    template<class T>
    constexpr T &&Forward(std::remove_reference_t<T> &&t) noexcept {
        return std::forward<T>(t);
    }

}
#endif