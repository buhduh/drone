#ifndef COMPONENT_INPUT_GAMEPAD_HPP
#define COMPONENT_INPUT_GAMEPAD_HPP

#include "stdint.h"

namespace component {
namespace input {

enum GAMEPAD_STATUS {
	OK, 
	NO_GAMEPAD
};

typedef uint16_t BUTTON;
typedef uint16_t BUTTON_STATE;
constexpr     BUTTON GAMEPAD_BUTTON_A            = (1 << 0);
constexpr     BUTTON GAMEPAD_BUTTON_B            = (1 << 1);
constexpr     BUTTON GAMEPAD_BUTTON_X            = (1 << 2);
constexpr     BUTTON GAMEPAD_BUTTON_Y            = (1 << 3);
constexpr     BUTTON GAMEPAD_BUTTON_LEFT_BUMPER  = (1 << 4);
constexpr     BUTTON GAMEPAD_BUTTON_RIGHT_BUMPER = (1 << 5);
constexpr     BUTTON GAMEPAD_BUTTON_BACK         = (1 << 6);
constexpr     BUTTON GAMEPAD_BUTTON_START        = (1 << 7);
constexpr     BUTTON GAMEPAD_BUTTON_GUIDE        = (1 << 8);
constexpr     BUTTON GAMEPAD_BUTTON_LEFT_THUMB   = (1 << 9);
constexpr     BUTTON GAMEPAD_BUTTON_RIGHT_THUMB  = (1 << 10);
constexpr     BUTTON GAMEPAD_BUTTON_DPAD_UP      = (1 << 11);
constexpr     BUTTON GAMEPAD_BUTTON_DPAD_RIGHT   = (1 << 12);
constexpr     BUTTON GAMEPAD_BUTTON_DPAD_DOWN    = (1 << 13);
constexpr     BUTTON GAMEPAD_BUTTON_DPAD_LEFT    = (1 << 14);

typedef uint8_t GAMEPAD_AXIS;
constexpr    GAMEPAD_AXIS GAMEPAD_AXIS_LEFT_X        = 0;
constexpr    GAMEPAD_AXIS GAMEPAD_AXIS_LEFT_Y        = 1;
constexpr    GAMEPAD_AXIS GAMEPAD_AXIS_RIGHT_X       = 2;
constexpr    GAMEPAD_AXIS GAMEPAD_AXIS_RIGHT_Y       = 3;
constexpr    GAMEPAD_AXIS GAMEPAD_AXIS_LEFT_TRIGGER  = 4;
constexpr    GAMEPAD_AXIS GAMEPAD_AXIS_RIGHT_TRIGGER = 5;

typedef float GAMEPAD_AXIS_VALUE;
constexpr uint8_t GAMEPAD_AXIS_COUNT = 6;

struct JoypadState {
	GAMEPAD_AXIS_VALUE axisState[GAMEPAD_AXIS_COUNT];
	//a bitmap
	BUTTON_STATE buttonState;
};

//TODO gonna have to wrap my head around c++ callbacks sooner or later
//probably use them for plugging in and out.
template <class T>
class Gamepad {
	public:
	Gamepad(T);
	GAMEPAD_STATUS getState(JoypadState&);
	private:
	T wrapper;
};

}
}
#endif
