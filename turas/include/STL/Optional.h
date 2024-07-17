#ifndef TURAS_STL_OPTIONAL_H
#define TURAS_STL_OPTIONAL_H

#include <optional>

namespace turas {
    template<typename T>
    using Optional = std::optional<T>;
}
#endif