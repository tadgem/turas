#pragma once

#include <memory>

#define TURAS_ENABLE_MEMORY_TRACKING
#ifdef TURAS_ENABLE_MEMORY_TRACKING
#include "STL/HashMap.h"
#include "STL/String.h"
#endif
namespace turas {
#ifdef TURAS_ENABLE_MEMORY_TRACKING
    struct AllocInfo {
        size_t count;
        size_t size;
    };

    class DebugMemoryTracker {
    public:
        DebugMemoryTracker();

        HashMap<String, AllocInfo> s_AllocationInfo;
        inline static DebugMemoryTracker *s_Instance = nullptr;
    };

#endif

    template<typename T>
    using UPtr = std::unique_ptr<T>;

    template<typename _Ty, typename ... Args>
    constexpr UPtr<_Ty> CreateUnique(Args &&... args) {
        return std::make_unique<_Ty>(std::forward<Args>(args)...);
    }

}

#ifdef TURAS_ENABLE_MEMORY_TRACKING
#define TURAS_IMPL_ALLOC(X) void * operator new(size_t size) \
{                                                            \
if(turas::DebugMemoryTracker::s_Instance->s_AllocationInfo.find(#X)==turas::DebugMemoryTracker::s_Instance->s_AllocationInfo.end()) {turas::DebugMemoryTracker::s_Instance->s_AllocationInfo.emplace(#X, AllocInfo{0,0});} \
turas::DebugMemoryTracker::s_Instance->s_AllocationInfo[#X].count++;                         \
turas::DebugMemoryTracker::s_Instance->s_AllocationInfo[#X].size += size;                                                             \
return malloc (size);\
}\
void operator delete(void* p)\
{                                                            \
free(p);                                                     \
if(!turas::DebugMemoryTracker::s_Instance) return;           \
turas::DebugMemoryTracker::s_Instance->s_AllocationInfo[#X].count--;                         \
turas::DebugMemoryTracker::s_Instance->s_AllocationInfo[#X].size -= sizeof(X);                                                                  \
}
#else
#define TURAS_IMPL_ALLOC(X)
#endif