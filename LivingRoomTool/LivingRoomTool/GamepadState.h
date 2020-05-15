#pragma once

#include "Common.h"
#include <magic_enum/magic_enum.hpp>

enum class GamepadButtons : uint32_t
{
	kNone			= 0,
	kLT				= 1 << 0,
	kLB				= 1 << 1,
	kLThumb			= 1 << 2,
	kLLeft			= 1 << 3,
	kLUp			= 1 << 4,
	kLRight			= 1 << 5,
	kLDown			= 1 << 6,
	kView			= 1 << 7,
	kMenu			= 1 << 8,
	kRT				= 1 << 9,
	kRB				= 1 << 10,
	kRThumb			= 1 << 11,
	kRLeft			= 1 << 12,
	kRUp			= 1 << 13,
	kRRight			= 1 << 14,
	kRDown			= 1 << 15,

	kLThumbLeft		= 1 << 16,
	kLThumbUp		= 1 << 17,
	kLThumbRight	= 1 << 18,
	kLThumbDown		= 1 << 19,
	kRThumbLeft		= 1 << 20,
	kRThumbUp		= 1 << 21,
	kRThumbRight	= 1 << 22,
	kRThumbDown		= 1 << 23,
};

GamepadButtons operator&(GamepadButtons a, GamepadButtons b);
GamepadButtons& operator|=(GamepadButtons& buttons, GamepadButtons val);
GamepadButtons operator|(GamepadButtons buttons, GamepadButtons val);

namespace GamepadButtonsHelper
{
	constexpr const uint32_t k_enumSize = 25;

	extern std::string_view ToString(GamepadButtons buttons);
	extern std::optional<GamepadButtons> FromString(const std::string_view& str);
	extern const std::array<std::string_view, k_enumSize>& GetLabels();
	extern const std::array<GamepadButtons, k_enumSize>& GetValues();

	extern bool IsAnalog(GamepadButtons buttons);
}

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

	GamepadButtons GetCurrentButtons() const { return m_currentButtons; }

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

	static const float k_analogAsGamepadButtonThreshold;

	friend class GamepadDevice;
};

