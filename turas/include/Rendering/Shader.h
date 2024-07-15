#pragma once
#include "lvk/Shader.h"
#include "STL/String.h"
#include "STL/Vector.h"

namespace turas {
    using ShaderStageBinary = Vector<u8>;

    enum class ShaderStageType
    {
        Vertex,
        Fragment,
        Compute
    };

    class Shader {
    public:
        String              m_ShaderName;
        String              m_ShaderPath;
        lvk::ShaderProgram  m_ShaderProgram;
    };
}