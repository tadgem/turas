#include "Core/System.h"
#include "Core/ECS.h"

entt::registry &turas::System::GetSceneRegistry(turas::Scene *scene) {
    return scene->p_Registry;
}
