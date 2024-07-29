//
// Created by liam_ on 7/29/2024.
//

#pragma once
#include "Core/Types.h"

namespace turas {
namespace Input {
	enum class ButtonState : u8
	{
		Pressed, // Button is active
		Released, // Button was released this frame
		NotPressed // Button not pressed
	};

	enum class MouseButton
	{
		LMB,
		RMB,
		Middle,
		Four,
		Five
	};


}
}