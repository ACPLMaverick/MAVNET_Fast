#include "GamepadState.h"

std::basic_string_view<char> GamepadButtonsConvert::ToString(GamepadButtons buttons)
{
	switch (buttons)
	{
	case GamepadButtons::kNone:
		return "kNone";
	case GamepadButtons::kLT:
		return "kLT";
	case GamepadButtons::kLB:
		return "kLB";
	case GamepadButtons::kLThumb:
		return "kLThumb";
	case GamepadButtons::kLLeft:
		return "kLLeft";
	case GamepadButtons::kLUp:
		return "kLUp";
	case GamepadButtons::kLRight:
		return "kLRight";
	case GamepadButtons::kLDown:
		return "kLDown";
	case GamepadButtons::kView:
		return "kView";
	case GamepadButtons::kMenu:
		return "kMenu";
	case GamepadButtons::kRT:
		return "kRT";
	case GamepadButtons::kRB:
		return "kRB";
	case GamepadButtons::kRThumb:
		return "kRThumb";
	case GamepadButtons::kRLeft:
		return "kRLeft";
	case GamepadButtons::kRUp:
		return "kRUp";
	case GamepadButtons::kRRight:
		return "kRRight";
	case GamepadButtons::kRDown:
		return "kRDown";
	case GamepadButtons::kLThumbLeft:
		return "kLThumbLeft";
	case GamepadButtons::kLThumbUp:
		return "kLThumbUp";
	case GamepadButtons::kLThumbRight:
		return "kLThumbRight";
	case GamepadButtons::kLThumbDown:
		return "kLThumbDown";
	case GamepadButtons::kRThumbLeft:
		return "kRThumbLeft";
	case GamepadButtons::kRThumbUp:
		return "kRThumbUp";
	case GamepadButtons::kRThumbRight:
		return "kRThumbRight";
	case GamepadButtons::kRThumbDown:
		return "kRThumbDown";
	default:
		LRT_Fail();
		return "";
	}
}

std::optional<GamepadButtons> GamepadButtonsConvert::FromString(const std::basic_string_view<char>& str)
{
	if (strcmp(str.data(), "kNone") == 0)
	{
		return GamepadButtons::kNone;
	}
	else if (strcmp(str.data(), "kLT") == 0)
	{
		return GamepadButtons::kLT;
	}
	else if (strcmp(str.data(), "kLB") == 0)
	{
		return GamepadButtons::kLB;
	}
	else if (strcmp(str.data(), "kLThumb") == 0)
	{
		return GamepadButtons::kLThumb;
	}
	else if (strcmp(str.data(), "kLLeft") == 0)
	{
		return GamepadButtons::kLLeft;
	}
	else if (strcmp(str.data(), "kLUp") == 0)
	{
		return GamepadButtons::kLUp;
	}
	else if (strcmp(str.data(), "kLRight") == 0)
	{
		return GamepadButtons::kLRight;
	}
	else if (strcmp(str.data(), "kLDown") == 0)
	{
		return GamepadButtons::kLDown;
	}
	else if (strcmp(str.data(), "kView") == 0)
	{
		return GamepadButtons::kView;
	}
	else if (strcmp(str.data(), "kMenu") == 0)
	{
		return GamepadButtons::kMenu;
	}
	else if (strcmp(str.data(), "kRT") == 0)
	{
		return GamepadButtons::kRT;
	}
	else if (strcmp(str.data(), "kRB") == 0)
	{
		return GamepadButtons::kRB;
	}
	else if (strcmp(str.data(), "kRThumb") == 0)
	{
		return GamepadButtons::kRThumb;
	}
	else if (strcmp(str.data(), "kRLeft") == 0)
	{
		return GamepadButtons::kRLeft;
	}
	else if (strcmp(str.data(), "kRUp") == 0)
	{
		return GamepadButtons::kRUp;
	}
	else if (strcmp(str.data(), "kRRight") == 0)
	{
		return GamepadButtons::kRRight;
	}
	else if (strcmp(str.data(), "kRDown") == 0)
	{
		return GamepadButtons::kRDown;
	}
	else if (strcmp(str.data(), "kLThumbLeft") == 0)
	{
		return GamepadButtons::kLThumbLeft;
	}
	else if (strcmp(str.data(), "kLThumbUp") == 0)
	{
		return GamepadButtons::kLThumbUp;
	}
	else if (strcmp(str.data(), "kLThumbRight") == 0)
	{
		return GamepadButtons::kLThumbRight;
	}
	else if (strcmp(str.data(), "kLThumbDown") == 0)
	{
		return GamepadButtons::kLThumbDown;
	}
	else if (strcmp(str.data(), "kRThumbLeft") == 0)
	{
		return GamepadButtons::kRThumbLeft;
	}
	else if (strcmp(str.data(), "kRThumbUp") == 0)
	{
		return GamepadButtons::kRThumbUp;
	}
	else if (strcmp(str.data(), "kRThumbRight") == 0)
	{
		return GamepadButtons::kRThumbRight;
	}
	else if (strcmp(str.data(), "kRThumbDown") == 0)
	{
		return GamepadButtons::kRThumbDown;
	}

	LRT_Fail();
	return std::optional<GamepadButtons>();
}

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

const float GamepadState::k_analogToDigitalThreshold = 0.9f;