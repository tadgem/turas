#include "STL/Memory.h"
#include "Core/Engine.h"
#include "STL/HashMap.h"
#include "STL/String.h"

void* operator new(size_t size)
{
    turas::DebugMemoryTracker::s_UntrackedSize += size;
    return malloc(size);
}

void operator delete(void* memory, size_t size)
{
    turas::DebugMemoryTracker::s_UntrackedSize -= size;
    free(memory);
}



int main(int argc, char** argv)
{
    turas::Engine app;
    app.Init();
    app.Run();
    app.Shutdown();
    return 0;
}