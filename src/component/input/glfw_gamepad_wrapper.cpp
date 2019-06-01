//TODO if nothing else, typedef std::allocator and change all stl calls for later
#include <vector>

#include "component/input/glfw_gamepad_wrapper.hpp"
#include "component/asset/asset.hpp"

namespace component {
namespace input {

bool GLFWGamepadWrapper::mappingLoaded(false);

GLFWGamepadWrapper::GLFWGamepadWrapper(GAMEPAD_STATUS& status) {
	connected = false;
	if(!mappingLoaded) {
		std::vector<char> db = 
			//component::asset::getSmallFileContents("assets/gamecontrollerdb.txt");
			asset::getSmallFileContents("assets/gamecontrollerdb.txt");
		glfwUpdateGamepadMappings(db.data());
		mappingLoaded = true;	
	}
	status = NO_GAMEPAD;
	//just gonna grab the first one
	for(uint16_t i = 0; i < GLFW_JOYSTICK_LAST; i++) {
		if(glfwJoystickPresent(i) && glfwJoystickIsGamepad(i)) {
			gamepadID = i;
			status = OK;
			connected = true;
			break;
		}
	}
	if(status != OK) {
		return;
	}
}

//do I care if it's not connected?
//consider only checking for buttons that have actions associated with them
GAMEPAD_STATUS GLFWGamepadWrapper::getState(JoypadState& state) {	
	BUTTON_STATE buttonS(0);
	glfwGetGamepadState(gamepadID, &glfwState);
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_A]) {;
		buttonS |= GAMEPAD_BUTTON_A;
	}
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_B]) {
		buttonS |= GAMEPAD_BUTTON_B;
	}
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_X]) {
		buttonS |= GAMEPAD_BUTTON_X;
	}
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_Y]) {
		buttonS |= GAMEPAD_BUTTON_Y;
	}
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER]) {
		buttonS |= GAMEPAD_BUTTON_LEFT_BUMPER;
	}
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER]) {
		buttonS |= GAMEPAD_BUTTON_RIGHT_BUMPER;
	}
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_BACK]) {
		buttonS |= GAMEPAD_BUTTON_BACK;
	}
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_START]) {
		buttonS |= GAMEPAD_BUTTON_START;
	}
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_GUIDE]) {
		buttonS |= GAMEPAD_BUTTON_GUIDE;
	}
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB]) {
		buttonS |= GAMEPAD_BUTTON_LEFT_THUMB;
	}
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB]) {
		buttonS |= GAMEPAD_BUTTON_RIGHT_THUMB;
	}
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]) {
		buttonS |= GAMEPAD_BUTTON_DPAD_UP;
	}
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]) {
		buttonS |= GAMEPAD_BUTTON_DPAD_RIGHT;
	}
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) {
		buttonS |= GAMEPAD_BUTTON_DPAD_DOWN;
	}
	if(glfwState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]) {
		buttonS |= GAMEPAD_BUTTON_DPAD_LEFT;
	}
	state.buttonState = buttonS;
	state.axisState[GAMEPAD_AXIS_RIGHT_TRIGGER] = glfwState.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
	state.axisState[GAMEPAD_AXIS_LEFT_X] = glfwState.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
	state.axisState[GAMEPAD_AXIS_LEFT_Y] = glfwState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
	state.axisState[GAMEPAD_AXIS_RIGHT_X] = glfwState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
	state.axisState[GAMEPAD_AXIS_RIGHT_Y] = glfwState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
	state.axisState[GAMEPAD_AXIS_LEFT_TRIGGER] = glfwState.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
	state.axisState[GAMEPAD_AXIS_RIGHT_TRIGGER] = glfwState.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
	return OK;
}

}
}
