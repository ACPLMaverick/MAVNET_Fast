#include "InputActionConverter.h"

#include "InputAction.h"

InputActionConverter::PlatformKey InputActionConverter::ToPlatformKey(InputActionKey key)
{
	return k_actionKeyToPlatform[static_cast<uint8_t>(key)];
}

InputActionKey InputActionConverter::FromPlatformKey(PlatformKey platformKey)
{
	return GetPlatformKeyToAction()[static_cast<size_t>(platformKey)];
}

InputActionKey* InputActionConverter::GetPlatformKeyToAction()
{
	static const size_t k_arraySize = std::numeric_limits<PlatformKey>::max();
	static InputActionKey k_platformKeyToAction[k_arraySize] = {};
	static bool k_platformKeyToActionInitialized = false;

	if (k_platformKeyToActionInitialized == false)
	{
		for (size_t i = 0; i < k_arraySize; ++i)
		{
			k_platformKeyToAction[k_actionKeyToPlatform[i]] = static_cast<InputActionKey>(i);
		}

		k_platformKeyToActionInitialized = true;
	}

	return k_platformKeyToAction;
}

#include <dinput.h>

const InputActionConverter::PlatformKey InputActionConverter::k_actionKeyToPlatform[] =
{
		0x00,			// kNone,
						// 
		DIK_ESCAPE,		// kEsc,
		DIK_F1,			// kF1,
		DIK_F2,			// kF2,
		DIK_F3,			// kF3,
		DIK_F4,			// kF4,
		DIK_F5,			// kF5,
		DIK_F6,			// kF6,
		DIK_F7,			// kF7,
		DIK_F8,			// kF8,
		DIK_F9,			// kF9,
		DIK_F10,		// kF10,
		DIK_F11,		// kF11,
		DIK_F12,		// kF12,
						// 
		DIK_GRAVE,		// kGrave,
		DIK_1,			// k1,
		DIK_2,			// k2,
		DIK_3,			// k3,
		DIK_4,			// k4,
		DIK_5,			// k5,
		DIK_6,			// k6,
		DIK_7,			// k7,
		DIK_8,			// k8,
		DIK_9,			// k9,
		DIK_0,			// k0,
		DIK_MINUS,		// kMinus,
		DIK_EQUALS,		// kPlus,
		DIK_LBRACKET,	// kLeftBracket,
		DIK_RBRACKET,	// kRightBracket,
		DIK_SEMICOLON,	// kSemicolon,
		DIK_APOSTROPHE,	// kApostrophe,
		DIK_BACKSLASH,	// kBackslash,
		DIK_COMMA,		// kComma,
		DIK_PERIOD,		// kPeriod,
		DIK_SLASH,		// kSlash,
						// 
		DIK_TAB,		// kTab,
		DIK_CAPSLOCK,	// kCapsLock,
		DIK_LSHIFT,		// kLeftShift,
		DIK_LCONTROL,	// kLeftCtrl,
		DIK_LWIN,		// kSystemKey,
		DIK_LALT,		// kLeftAlt,
		DIK_SPACE,		// kSpace,
		DIK_RALT,		// kRightAlt,
		DIK_RMENU,		// kMenu,
		DIK_RCONTROL,	// kRightCtrl,
		DIK_RSHIFT,		// kRightShift,
		DIK_RETURN,		// kEnter,
		DIK_BACKSPACE,	// kBackspace,
						// 
		DIK_Q,			// kQ,
		DIK_W,			// kW,
		DIK_E,			// kE,
		DIK_R,			// kR,
		DIK_T,			// kT,
		DIK_Y,			// kY,
		DIK_U,			// kU,
		DIK_I,			// kI,
		DIK_O,			// kO,
		DIK_P,			// kP,
		DIK_A,			// kA,
		DIK_S,			// kS,
		DIK_D,			// kD,
		DIK_F,			// kF,
		DIK_G,			// kG,
		DIK_H,			// kH,
		DIK_J,			// kJ,
		DIK_K,			// kK,
		DIK_L,			// kL,
		DIK_Z,			// kZ,
		DIK_X,			// kX,
		DIK_C,			// kC,
		DIK_V,			// kV,
		DIK_B,			// kB,
		DIK_N,			// kN,
		DIK_M,			// kM,
						// 
		DIK_SYSRQ,		// kPrintScreen,
		DIK_SCROLL,		// kScrollLock,
		DIK_PAUSE,		// kPauseBreak,
		DIK_INSERT,		// kInsert,
		DIK_HOME,		// kHome,
		DIK_PGUP,		// kPageUp,
		DIK_DELETE,		// kDelete,
		DIK_END,		// kEnd,
		DIK_PGDN,		// kPageDown,
						// 
		DIK_UPARROW,	// kArrowUp,
		DIK_RIGHTARROW,	// kArrowRight,
		DIK_DOWNARROW,	// kArrowDown,
		DIK_LEFTARROW	// kArrowLeft,
};