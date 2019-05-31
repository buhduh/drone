#ifndef COMPONENT_INPUT_GAMEPAD_HPP
#define COMPONENT_INPUT_GAMEPAD_HPP

namespace component {
namespace input {


typedef uint16_t BUTTON;
static const     BUTTON GAMEPAD_BUTTON_A            = (1 << 0);
static const     BUTTON GAMEPAD_BUTTON_B            = (1 << 1);
static const     BUTTON GAMEPAD_BUTTON_X            = (1 << 2);
static const     BUTTON GAMEPAD_BUTTON_Y            = (1 << 3);
static const     BUTTON GAMEPAD_BUTTON_LEFT_BUMPER  = (1 << 4);
static const     BUTTON GAMEPAD_BUTTON_RIGHT_BUMPER = (1 << 5);
static const     BUTTON GAMEPAD_BUTTON_BACK         = (1 << 6);
static const     BUTTON GAMEPAD_BUTTON_START        = (1 << 7);
static const     BUTTON GAMEPAD_BUTTON_GUIDE        = (1 << 8);
static const     BUTTON GAMEPAD_BUTTON_LEFT_THUMB   = (1 << 9);
static const     BUTTON GAMEPAD_BUTTON_RIGHT_THUMB  = (1 << 10);
static const     BUTTON GAMEPAD_BUTTON_DPAD_UP      = (1 << 11);
static const     BUTTON GAMEPAD_BUTTON_DPAD_RIGHT   = (1 << 12);
static const     BUTTON GAMEPAD_BUTTON_DPAD_DOWN    = (1 << 13);
static const     BUTTON GAMEPAD_BUTTON_DPAD_LEFT    = (1 << 14);

typedef uint8_t BUTTON_STATE;
//prefer "not pressed", "released" implies a change in state
//this interface does not report on changes
static const BUTTON_STATE NOT_PRESSED = 0;
static const BUTTON_STATE PRESSED     = 1;

typedef uint8_t GAMEPAD_AXIS;
static const    GAMEPAD_AXIS GAMEPAD_AXIS_LEFT_X        = 0;
static const    GAMEPAD_AXIS GAMEPAD_AXIS_LEFT_Y        = 1;
static const    GAMEPAD_AXIS GAMEPAD_AXIS_RIGHT_X       = 2;
static const    GAMEPAD_AXIS GAMEPAD_AXIS_RIGHT_Y       = 3;
static const    GAMEPAD_AXIS GAMEPAD_AXIS_LEFT_TRIGGER  = 4;
static const    GAMEPAD_AXIS GAMEPAD_AXIS_RIGHT_TRIGGER = 5;

typedef float   GAMEPAD_AXIS_VALUE;
static const uint8_t GAMEPAD_AXIS_COUNT = 6;

struct JoypadState {
	AXIS_VALUE[GAMEPAD_AXIS_COUNT] axisState;
	//a bitmap
	uint16_t      buttonState;
};

/*
TODO bitwise operations for error codes?
typedef uint8_t JOYPAD_FLAGS;
static const JOYPAD_FLAG OK         (1 << 0);
static const JOYPAD_FLAG NO_GAMEPAD (1 << 0);
*/

//TODO gonna have to wrap my head around c++ callbacks sooner or later
//probably use them for plugging in and out.
template <class T>
class Gamepad<T> {
	public:
	//TODO error flags
	getState(JoypadState&);
};

template <class T>
using wrapper = T;
void Gamepad::getState(JoypadState& state) {
	T::getState(state);	
}

}
}
#endif
