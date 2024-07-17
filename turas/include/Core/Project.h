//
// Created by liam_ on 7/3/2024.
//

#ifndef TURAS_ALL_PROJECT_H
#define TURAS_ALL_PROJECT_H

#include "STL/Memory.h"
#include "STL/HashMap.h"
#include "Core/Serialization.h"

namespace turas {
    // Project to encompass game-specfific data in engine
    class Project {
    public:
        TURAS_IMPL_ALLOC(Project)

        Project(const String &name);

        // store project name
        String m_Name;
        // store default project scene
        String m_DefaultScene;
        // store all serialized scene names - paths
        HashMap<String, String> m_SerializedScenes;

        bool AddScene(const String &name, const String &path);

        bool UpdateScene(const String &name, const String &path);

        bool RemoveScene(const String &name);

        bool SetDefaultScene(const String &name);

        template<typename Archive>
        void serialize(Archive &ar) {
            ar(m_Name, m_DefaultScene, m_SerializedScenes);
        }

    };
}
#endif //TURAS_ALL_PROJECT_H
