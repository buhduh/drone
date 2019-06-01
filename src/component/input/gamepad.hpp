#include "component/input/gamepade.hpp"

namespace component {
namespace input {

template <class T>
Gamepad::Gamepad(T)	: wrapper(T) {}

GAMEPAD_STATUS Gamepad::getState(JoypadState& state) {
	return wrapper.getState(state);	
}

}
}
