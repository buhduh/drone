#include <iostream>

#include <GLFW/glfw3.h>

#include "utils/utils.hpp"


typedef uint8_t FLAG;
static const FLAG OK                   = (1 << 0);
static const FLAG NO_GAMEPAD           = (1 << 1);

int main() {
	glfwInit();
	/*
	Would have preferred throwing the GLFW_JOYSTICK_* values in a static array
	but the compiler was complaining, probably a c++/c incompatability
	and I don't care enough to correct
	*/
	//The first one found should do
	//Must be a "gamepad", not supporting "joysticks" for now
	FLAG flag(NO_GAMEPAD);
	uint16_t joystickID;
	//TODO implement a better asset loader
	//TODO subtree this bullshit when i have the patience to deal with
	//idiotic git tutorials
	//cloned from: https://github.com/gabomdq/SDL_GameControllerDB.git
	auto mappings = utils::readFile("assets/gamecontrollerdb.txt");
	glfwUpdateGamepadMappings(mappings.data());
	for(uint16_t i = 0; i < GLFW_JOYSTICK_LAST; i++) {
		if(glfwJoystickPresent(i) && glfwJoystickIsGamepad(i)) {
			joystickID = i;
			flag = OK;
			break;
		}
	}
	if(flag & NO_GAMEPAD) {
		std::cerr << "no joystick found" << std::endl;
		return 0;
	}
	std::cout << "found a gamepad id: " << joystickID << std::endl;	
	std::cout << "name: " << glfwGetGamepadName(joystickID) << std::endl;
}
