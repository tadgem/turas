#include "Core/Project.h"

turas::Project::Project(const turas::String &name) : m_Name(name) {

}

bool turas::Project::AddScene(const turas::String &name, const turas::String &path) {
    if (m_SerializedScenes.find(name) != m_SerializedScenes.end()) {
        return false;
    }
    m_SerializedScenes.emplace(name, path);
    return true;
}

bool turas::Project::RemoveScene(const turas::String &name) {
    if (m_SerializedScenes.find(name) == m_SerializedScenes.end()) {
        return false;
    }
    m_SerializedScenes.erase(name);
    return true;
}

bool turas::Project::UpdateScene(const turas::String &name, const turas::String &path) {
    if (m_SerializedScenes.find(name) == m_SerializedScenes.end()) {
        return false;
    }
    m_SerializedScenes[name] = path;
    return true;
}

bool turas::Project::SetDefaultScene(const turas::String &name) {
    if (m_SerializedScenes.find(name) == m_SerializedScenes.end()) {
        return false;
    }
    m_DefaultScene = name;
    return true;
}
