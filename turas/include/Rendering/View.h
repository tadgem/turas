//
// Created by liam_ on 7/5/2024.
//

#pragma once

#include "Core/ECS.h"
#include "Core/Serialization.h"
#include "Core/Types.h"
#include "Debug/Profile.h"
#include "STL/Memory.h"
#include "STL/String.h"
#include "glm/glm.hpp"

namespace turas {

struct Camera {
  enum ProjectionType : u8 { Perspective, Orthographic };

  float m_FOV;
  float m_NearPlane;
  float m_FarPlane;
  u32 m_Width;
  u32 m_Height;
  ProjectionType m_ProjectionType;
  glm::mat4 m_ViewMatrix;
  glm::mat4 m_ProjectionMatrix;

  template <typename Archive> void serialize(Archive &ar) {
    ZoneScoped;
    ar(m_FOV, m_NearPlane, m_FarPlane, m_Width, m_Height, m_ProjectionType);
  }

  TURAS_IMPL_ALLOC(Camera)
};

class View {
public:
  enum class Type { Game, Debug };

  String m_Name;
  u64 m_Hash;
  Type m_Type;
  Entity m_CameraEntity;

  View(const String &name);

  template <typename Archive> void serialize(Archive &ar) {
    ZoneScoped;
    ar(m_Name);
  }

  TURAS_IMPL_ALLOC(View)
};
} // namespace turas