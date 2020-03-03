#include "GamepadState.h"

GamepadState::GamepadState()
{
	memset(this, 0, sizeof(GamepadState));
}

bool GamepadState::AreButtonsHeld(Buttons buttons) const
{
	return (m_currentButtons & buttons) == buttons;
}

bool GamepadState::AreButtonsPressed(Buttons buttons) const
{
	return (m_currentButtons & buttons) == buttons && (m_prevButtons & buttons) == 0;
}

bool GamepadState::AreButtonsReleased(Buttons buttons) const
{
	return (m_currentButtons & buttons) == 0 && (m_prevButtons & buttons) == buttons;
}