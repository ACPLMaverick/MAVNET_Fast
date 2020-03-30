#pragma once

#include "Common.h"

#include <magic_enum/magic_enum.hpp>

enum class InputActionKey : uint8_t
{
	kNone,

	// Keyboard
	kEsc,
	kF1,
	kF2,
	kF3,
	kF4,
	kF5,
	kF6,
	kF7,
	kF8,
	kF9,
	kF10,
	kF11,
	kF12,

	kGrave,
	k1,
	k2,
	k3,
	k4,
	k5,
	k6,
	k7,
	k8,
	k9,
	k0,
	kMinus,
	kPlus,
	kLeftBracket,
	kRightBracket,
	kSemicolon,
	kApostrophe,
	kBackslash,
	kComma,
	kPeriod,
	kSlash,

	kTab,
	kCapsLock,
	kLeftShift,
	kLeftCtrl,
	kSystemKey,
	kLeftAlt,
	kSpace,
	kRightAlt,
	kMenu,
	kRightCtrl,
	kRightShift,
	kEnter,
	kBackspace,

	kQ,
	kW,
	kE,
	kR,
	kT,
	kY,
	kU,
	kI,
	kO,
	kP,
	kA,
	kS,
	kD,
	kF,
	kG,
	kH,
	kJ,
	kK,
	kL,
	kZ,
	kX,
	kC,
	kV,
	kB,
	kN,
	kM,

	kPrintScreen,
	kScrollLock,
	kPauseBreak,
	kInsert,
	kHome,
	kPageUp,
	kDelete,
	kEnd,
	kPageDown,

	kArrowUp,
	kArrowRight,
	kArrowDown,
	kArrowLeft,

	// Mouse
	kMouseUp,
	kMouseDown,
	kMouseLeft,
	kMouseRight,
	kMouseLMB,
	kMouseMMB,
	kMouseRMB,
	kMouseScrollUp,
	kMouseScrollDown,

	ENUM_SIZE
};

namespace InputActionKeyHelper
{
	extern const InputActionKey g_MouseFirst;
	extern const InputActionKey g_MouseLast;
}

namespace magic_enum
{
	template <>
	struct enum_range<InputActionKey>
	{
		static constexpr int min = 0; // Must be greater than `INT16_MIN`.
		static constexpr int max = 255; // Must be less than `INT16_MAX`.
	};
}

class InputAction
{
public:

	InputAction(InputActionKey key, bool bPressed);
	InputAction(InputActionKey key, int32_t value);

	InputActionKey GetKey() const { return m_key; }
	int GetInt() const { return m_value.m_IntValue; }
	bool GetIsPressed() const { return m_value.m_bPressed; }

private:

	InputActionKey m_key;
	union
	{
		int32_t m_IntValue;
		bool m_bPressed;
	} m_value;
};