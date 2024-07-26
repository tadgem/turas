#pragma once
#include "Core/Serialization.h"
#include "Core/Types.h"
#include "STL/String.h"
#include "STL/Vector.h"
#include "lvk/Shader.h"
namespace turas
{
	using ShaderBinary = Vector<u8>;
	class Shader
	{
	public:
		String			   m_ShaderName;
		u64				   m_ShaderHash;
		lvk::ShaderProgram m_ShaderProgram;
		Vector<String>	   m_StagePaths;
	};
} // namespace turas
