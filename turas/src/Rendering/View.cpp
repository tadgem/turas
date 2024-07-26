//
// Created by liam_ on 7/10/2024.
//
#include "Rendering/View.h"
#include "Core/Log.h"
#include "Core/Utils.h"
#include "Systems/Camera.h"
turas::View::			View(const String& name) : m_Name(name), m_Hash(Utils::Hash(name)) {}
turas::SceneCameraView::SceneCameraView(const String& name, Optional<Entity> e, Scene* scene) : View(name), m_Scene(scene)
{
	if (e.has_value()) {
		m_CameraEntity = e.value();
	}
}
glm::mat4 turas::SceneCameraView::GetViewMatrix()
{
	if (m_Scene == nullptr) {
		log::warn("SceneCameraView : No scene has been provided, can't get the "
				  "camera's view matrix");
		return glm::mat4(1.0);
	}
	if (!m_Scene->GetRegistry().valid(m_CameraEntity.m_Handle)) {
		log::warn("SceneCameraView : Scene does not have the entity {}, can't get "
				  "the camera's view matrix",
				  (u32)m_CameraEntity.m_Handle);
		return glm::mat4(1.0);
	}
	if (!m_Scene->GetRegistry().any_of<CameraComponent>(m_CameraEntity.m_Handle)) {
		log::warn("SceneCameraView : Entity {} does not have a camera component, "
				  "can't get the camera's view matrix",
				  (u32)m_CameraEntity.m_Handle);
		return glm::mat4(1.0);
	}
	return m_Scene->GetRegistry().get<CameraComponent>(m_CameraEntity.m_Handle).m_Camera.m_ViewMatrix;
}
glm::mat4 turas::SceneCameraView::GetProjectionMatrix()
{
	if (m_Scene == nullptr) {
		log::warn("SceneCameraView : No scene has been provided, can't get the "
				  "camera's projection matrix");
		return glm::mat4(1.0);
	}
	if (!m_Scene->GetRegistry().valid(m_CameraEntity.m_Handle)) {
		log::warn("SceneCameraView : Scene does not have the entity {}, can't get "
				  "the camera's projection matrix",
				  (u32)m_CameraEntity.m_Handle);
		return glm::mat4(1.0);
	}
	if (!m_Scene->GetRegistry().any_of<CameraComponent>(m_CameraEntity.m_Handle)) {
		log::warn("SceneCameraView : Entity {} does not have a camera component, "
				  "can't get the camera's projection matrix",
				  (u32)m_CameraEntity.m_Handle);
		return glm::mat4(1.0);
	}
	return m_Scene->GetRegistry().get<CameraComponent>(m_CameraEntity.m_Handle).m_Camera.m_ProjectionMatrix;
}
turas::DebugCameraView::DebugCameraView(const turas::String& name) : View(name) {}
glm::mat4				turas::DebugCameraView::GetViewMatrix()
{
	m_DebugCamera.UpdateMatrices(m_Position, m_Rotation);
	return m_DebugCamera.m_ViewMatrix;
}
glm::mat4 turas::DebugCameraView::GetProjectionMatrix()
{
	m_DebugCamera.UpdateMatrices(m_Position, m_Rotation);
	return m_DebugCamera.m_ProjectionMatrix;
}
void turas::Camera::UpdateMatrices(glm::vec3 position, glm::vec3 eulerAngles) {}
