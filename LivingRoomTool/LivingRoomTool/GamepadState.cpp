#include "GamepadState.h"

GamepadState::GamepadState()
{
	memset(this, 0, sizeof(GamepadState));
}

bool GamepadState::AreButtonsHeld(GamepadButtons buttons) const
{
	return (m_currentButtons & buttons) == buttons;
}

bool GamepadState::AreButtonsPressed(GamepadButtons buttons) const
{
	return (m_currentButtons & buttons) == buttons && (m_prevButtons & buttons) == GamepadButtons::kNone;
}

bool GamepadState::AreButtonsReleased(GamepadButtons buttons) const
{
	return (m_currentButtons & buttons) == GamepadButtons::kNone && (m_prevButtons & buttons) == buttons;
}

GamepadButtons operator&(GamepadButtons a_a, GamepadButtons a_b)
{
	return static_cast<GamepadButtons>(static_cast<int>(a_a) & static_cast<int>(a_b));
}

GamepadButtons & operator|=(GamepadButtons & a_buttons, GamepadButtons a_val)
{
	return a_buttons = static_cast<GamepadButtons>(static_cast<int>(a_buttons) | static_cast<int>(a_val));
}

GamepadButtons operator|(GamepadButtons a_a, GamepadButtons a_b)
{
	return static_cast<GamepadButtons>(static_cast<uint16_t>(a_a) | static_cast<uint16_t>(a_b));
}
