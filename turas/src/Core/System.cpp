#include "Core/System.h"
#include "Core/ECS.h"

entt::registry &turas::System::GetSceneRegistry(turas::Scene *scene) {
    ZoneScoped;
    return scene->p_Registry;
}
