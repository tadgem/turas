#include "Core/ECS.h"
#include "Core/Engine.h"
#include "EditorEngine.h"
#include "STL/HashMap.h"
#include "STL/Memory.h"
#include "STL/String.h"
#include "Systems/Camera.h"
#include "Systems/EntityData.h"
#include "Systems/Mesh.h"
#include "Systems/Transform.h"

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
    turas::EditorEngine app{};
    app.AddSystem<turas::TransformSystem>();
    app.AddSystem<turas::MeshSystem>();
    app.AddSystem<turas::CameraSystem>();
    app.AddSystem<turas::EntityDataSystem>();
    app.Init();
	app.Run();
    app.Shutdown();
    return 0;
}