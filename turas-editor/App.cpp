#include "STL/Memory.h"
#include "Core/Engine.h"
#include "STL/HashMap.h"
#include "STL/String.h"
#include "Systems/Transform.h"
#include "Systems/Mesh.h"
#include "Systems/Camera.h"
#include "Systems/EntityData.h"
#include "Core/ECS.h"

#ifdef TURAS_ENABLE_MEMORY_TRACKING
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
#endif

int main(int argc, char** argv)
{
    turas::Engine app(true);
    app.AddSystem<turas::TransformSystem>();
    app.AddSystem<turas::MeshSystem>();
    app.AddSystem<turas::CameraSystem>();
    app.AddSystem<turas::EntityDataSystem>();
    app.Init();
    app.Run();
    app.Shutdown();
    return 0;
}