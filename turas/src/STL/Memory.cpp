
#include "STL/Memory.h"
#include "STL/HashMap.h"
#include "STL/String.h"

#ifdef TURAS_ENABLE_MEMORY_TRACKING
turas::DebugMemoryTracker::DebugMemoryTracker()
{
    if(s_Instance != nullptr)
    {
        // error;
        return;
    }

    s_Instance = this;
}
#endif