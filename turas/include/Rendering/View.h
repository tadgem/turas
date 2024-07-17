//
// Created by liam_ on 7/5/2024.
//

#pragma once

#include "STL/String.h"
#include "STL/Memory.h"
#include "Core/Types.h"
#include "glm/glm.hpp"
#include "Core/Serialization.h"
#include "Debug/Profile.h"

namespace turas {


    struct Camera {
        enum ProjectionType : u8 {
            Perspective,
            Orthographic
        };

        float m_FOV;
        float m_NearPlane;
        float m_FarPlane;
        u32 m_Width;
        u32 m_Height;
        ProjectionType m_ProjectionType;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ProjectionMatrix;

        template<typename Archive>
        void serialize(Archive &ar) {
            ZoneScoped;
            ar(m_FOV, m_NearPlane, m_FarPlane, m_Width, m_Height, m_ProjectionType);
        }

        TURAS_IMPL_ALLOC(Camera)
    };

    class View {
    public:

        String m_Name;
        u64 m_Hash;

        View(const String &name);

        template<typename Archive>
        void serialize(Archive &ar) {
            ZoneScoped;
            ar(m_Name);
        }

        TURAS_IMPL_ALLOC(View)

    };
}