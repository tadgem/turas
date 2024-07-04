#include "STL/Memory.h"
#include "Core/Engine.h"
#include "STL/HashMap.h"
#include "STL/String.h"

int main(int argc, char** argv)
{
    turas::Engine app;
    app.Init();

    if(turas::DebugMemoryTracker::s_Instance->s_AllocationInfo.find("Scene") == turas::DebugMemoryTracker::s_Instance->s_AllocationInfo.end())
    {
        // fuck;
        int i = 1;
    }

    app.Shutdown();
    return 0;
}