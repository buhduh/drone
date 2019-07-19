#include "component/input/gamepad.hpp"

namespace component {
namespace input {

template <class T>
Gamepad<T>::Gamepad(T wrapper) : wrapper(wrapper) {}

template <class T>
GAMEPAD_STATUS Gamepad<T>::getState(JoypadState& state) {
	return wrapper.getState(state);	
}

}
}
