#pragma once

#include "lvk/Shader.h"
#include "STL/String.h"
#include "STL/Vector.h"
#include "Core/Serialization.h"
#include "Core/Types.h"

namespace turas {

    using ShaderBinary = Vector<u8>;

    class Shader {
    public:
        String m_ShaderName;
        lvk::ShaderProgram m_ShaderProgram;
        Vector<String> m_StagePaths;
    };
}