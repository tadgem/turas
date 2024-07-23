//
// Created by liam_ on 7/5/2024.
//

#pragma once

#include "Core/ECS.h"
#include "Core/Serialization.h"
#include "Core/Types.h"
#include "Debug/Profile.h"
#include "STL/Memory.h"
#include "STL/Optional.h"
#include "STL/String.h"
#include "glm/glm.hpp"

namespace turas
{
  struct Camera
  {
    enum ProjectionType : u8 { Perspective, Orthographic };

    float m_FOV;
    float m_NearPlane;
    float m_FarPlane;
    u32 m_Width;
    u32 m_Height;
    ProjectionType m_ProjectionType;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;

    template <typename Archive>
    void serialize(Archive& ar)
    {
      ZoneScoped;
      ar(m_FOV, m_NearPlane, m_FarPlane, m_Width, m_Height, m_ProjectionType);
    }

    TURAS_IMPL_ALLOC(Camera)
  };

  class View
  {
  public:

    String m_Name;
    u64 m_Hash;

    View(const String& name);

    virtual glm::mat4 GetViewMatrix() = 0;
    virtual glm::mat4 GetProjectionMatrix() = 0;

    TURAS_IMPL_ALLOC(View)
  };

  class SceneCameraView : public View
  {
  public:

    SceneCameraView(const String& name, Optional<Entity> e = {}, Scene* scene = nullptr);

    Entity  m_CameraEntity;
    Scene*  m_Scene;

    glm::mat4 GetViewMatrix() override;
    glm::mat4 GetProjectionMatrix() override;
  };
} // namespace turas
