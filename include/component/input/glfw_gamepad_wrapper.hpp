#ifndef COMPONENT_INPUT_GLFW_GAMEPAD_WRAPPER_HPP
#define COMPONENT_INPUT_GLFW_GAMEPAD_WRAPPER_HPP

#include <GLFW/glfw3.h>

#include "component/input/gamepad.hpp"

namespace component {
namespace input {

/*
This makes no attempt at thread safety.
GLFW states in many places that gamepad interaction is done through the main thread.
*/
class GLFWGamepadWrapper {
	public:
	GLFWGamepadWrapper(GAMEPAD_STATUS&);
	GAMEPAD_STATUS getState(JoypadState&);
	private:
	static bool mappingLoaded;
	GLFWgamepadstate glfwState;
	uint16_t gamepadID;
	bool connected;
};

}
}
#endif
