#pragma once

#include "Common.h"

class GamepadState
{
public:
	enum Buttons
	{
		Button_None = 0,
		Button_LT = 1 << 0,
		Button_LB = 1 << 1,
		Button_LThumb = 1 << 2,
		Button_LLeft = 1 << 3,
		Button_LUp = 1 << 4,
		Button_LRight = 1 << 5,
		Button_LDown = 1 << 6,
		Button_View = 1 << 7,
		Button_Menu = 1 << 8,
		Button_RT = 1 << 9,
		Button_RB = 1 << 10,
		Button_RThumb = 1 << 11,
		Button_RLeft = 1 << 12,
		Button_RUp = 1 << 13,
		Button_RRight = 1 << 14,
		Button_RDown = 1 << 15
	};

	struct Thumb
	{
		float AxisX;
		float AxisY;
	};

	struct Trigger
	{
		float Axis;
	};

public:

	GamepadState();

	GamepadState(const GamepadState&) = default;
	GamepadState& operator=(const GamepadState&) = default;

	bool AreButtonsHeld(Buttons buttons) const;
	bool AreButtonsPressed(Buttons buttons) const;
	bool AreButtonsReleased(Buttons buttons) const;

	const Thumb& GetLeftThumb() const { return m_leftThumb; }
	const Thumb& GetRightThumb() const { return m_rightThumb; }
	const Trigger& GetLeftTrigger() const { return m_leftTrigger; }
	const Trigger& GetRightTrigger() const { return m_rightTrigger; }

private:

	Buttons m_currentButtons;
	Buttons m_prevButtons;

	Thumb m_leftThumb;
	Thumb m_rightThumb;
	Trigger m_leftTrigger;
	Trigger m_rightTrigger;
};

