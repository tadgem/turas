#pragma once
#include "lvk/Shader.h"
#include "STL/String.h"
#include "STL/Vector.h"
#include "Core/Serialization.h"

namespace turas {

    struct ShaderBinary
    {
        Vector<u8>  m_Binary;
        String      m_Path;
    };

    class Shader {
    public:
        String              m_ShaderName;
        lvk::ShaderProgram  m_ShaderProgram;
        Vector<String>      m_StagePaths;
    };
}