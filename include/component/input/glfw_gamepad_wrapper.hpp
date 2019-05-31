#ifndef COMPONENT_INPUT_GLFW_GAMEPAD_WRAPPER_HPP
#define COMPONENT_INPUT_GLFW_GAMEPAD_WRAPPER_HPP

#include "component/input/gamepad.hpp"

namespace component {
namespace input {

/*
This makes no attempt at thread safety.
GLFW states in many places that gamepad interaction is done through the main thread.
*/
class GLFWGamepadWrapper {
	public:
	GLFWGamepadWrapper();
	void getState(JoypadState&);
	private:
	static bool mappingLoaded;
};

}
}
#endif
