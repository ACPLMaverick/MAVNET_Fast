#pragma once

#include "Common.h"

enum class GamepadButtons : uint16_t
{
	kNone	= 0,
	kLT		= 1 << 0,
	kLB		= 1 << 1,
	kLThumb	= 1 << 2,
	kLLeft	= 1 << 3,
	kLUp	= 1 << 4,
	kLRight	= 1 << 5,
	kLDown	= 1 << 6,
	kView	= 1 << 7,
	kMenu	= 1 << 8,
	kRT		= 1 << 9,
	kRB		= 1 << 10,
	kRThumb	= 1 << 11,
	kRLeft	= 1 << 12,
	kRUp	= 1 << 13,
	kRRight	= 1 << 14,
	kRDown	= 1 << 15
};

GamepadButtons operator&(GamepadButtons a, GamepadButtons b);
GamepadButtons& operator|=(GamepadButtons& buttons, GamepadButtons val);
GamepadButtons operator|(GamepadButtons buttons, GamepadButtons val);

class GamepadState
{
public:

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

	bool AreButtonsHeld(GamepadButtons buttons) const;
	bool AreButtonsPressed(GamepadButtons buttons) const;
	bool AreButtonsReleased(GamepadButtons buttons) const;

	const Thumb& GetLeftThumb() const { return m_leftThumb; }
	const Thumb& GetRightThumb() const { return m_rightThumb; }
	const Trigger& GetLeftTrigger() const { return m_leftTrigger; }
	const Trigger& GetRightTrigger() const { return m_rightTrigger; }

private:

	GamepadButtons m_currentButtons;
	GamepadButtons m_prevButtons;

	Thumb m_leftThumb;
	Thumb m_rightThumb;
	Trigger m_leftTrigger;
	Trigger m_rightTrigger;

	static const float k_analogToDigitalThreshold;

	friend class GamepadDevice;
};

