#ifndef TURAS_STL_MEMORY_H
#define TURAS_STL_MEMORY_H
#include <memory>

#define ENABLED_MEMORY_TRACKER
#if defined ENABLED_MEMORY_TRACKER
#undef new

void *operator new (size_t size, char *file, int line, char *function);
// other operators

#define NEW new (__FILE__, __LINE__, __FUNCTION__)
#else
#define NEW new
#endif

namespace turas
{
    template<typename T>
    using UPtr = std::unique_ptr<T>;


    template<typename T, typename ... Args>
    constexpr UPtr<T> CreateUnique(Args &&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

}

#endif