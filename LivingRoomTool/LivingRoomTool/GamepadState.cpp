#include "GamepadState.h"

namespace GamepadButtonsHelper
{
	static_assert(static_cast<uint32_t>(GamepadButtons::kRThumbDown) == (1 << (k_enumSize - 1 - 1)));

	static const std::array<std::string_view, k_enumSize> k_labels = 
	{
		"kNone"
		, "kLT"
		, "kLB"
		, "kLThumb"
		, "kLLeft"
		, "kLUp"
		, "kLRight"
		, "kLDown"
		, "kView"
		, "kMenu"
		, "kRT"
		, "kRB"
		, "kRThumb"
		, "kRLeft"
		, "kRUp"
		, "kRRight"
		, "kRDown"
		, "kLThumbLeft"
		, "kLThumbUp"
		, "kLThumbRight"
		, "kLThumbDown"
		, "kRThumbLeft"
		, "kRThumbUp"
		, "kRThumbRight"
		, "kRThumbDown"
	};

	static const std::array<GamepadButtons, k_enumSize> k_values = 
	{
		GamepadButtons::kNone,
		GamepadButtons::kLT,
		GamepadButtons::kLB,
		GamepadButtons::kLThumb,
		GamepadButtons::kLLeft,
		GamepadButtons::kLUp,
		GamepadButtons::kLRight,
		GamepadButtons::kLDown,
		GamepadButtons::kView,
		GamepadButtons::kMenu,
		GamepadButtons::kRT,
		GamepadButtons::kRB,
		GamepadButtons::kRThumb,
		GamepadButtons::kRLeft,
		GamepadButtons::kRUp,
		GamepadButtons::kRRight,
		GamepadButtons::kRDown,
		GamepadButtons::kLThumbLeft,
		GamepadButtons::kLThumbUp,
		GamepadButtons::kLThumbRight,
		GamepadButtons::kLThumbDown,
		GamepadButtons::kRThumbLeft,
		GamepadButtons::kRThumbUp,
		GamepadButtons::kRThumbRight,
		GamepadButtons::kRThumbDown,
	};

	// Note that this will fail on merged bit flags.
	static constexpr uint32_t GetOrdinalOfEnum(GamepadButtons buttons)
	{
		uint32_t value = static_cast<uint32_t>(buttons);
		uint32_t num = 0;
		while (value != 0)
		{
			value = value >> 1;
			++num;
		}
		return num;
	}

	static constexpr const std::string_view& GetNameByEnum(GamepadButtons buttons)
	{
		return k_labels[GetOrdinalOfEnum(buttons)];
	}

	static constexpr GamepadButtons GetEnumByName(const std::string_view& str)
	{
		uint32_t num = 0;
		for (const std::string_view& staticStr : k_labels)
		{
			if (staticStr.compare(str) == 0)
			{
				break;
			}

			++num;
		}

		if (num == 0)
		{
			return static_cast<GamepadButtons>(num);
		}
		else if (num < k_enumSize)
		{
			return static_cast<GamepadButtons>(1 << (num - 1));
		}
		else
		{
			return static_cast<GamepadButtons>(-1);
		}
	}

	std::string_view ToString(GamepadButtons buttons)
	{
		return GetNameByEnum(buttons);
	}

	std::optional<GamepadButtons> FromString(const std::string_view& str)
	{
		const GamepadButtons buttons = GetEnumByName(str);
		if (buttons == static_cast<GamepadButtons>(-1))
		{
			return std::optional<GamepadButtons>();
		}
		else
		{
			return std::optional(buttons);
		}
	}

	const std::array<std::string_view, k_enumSize>& GetLabels()
	{
		return k_labels;
	}

	const std::array<GamepadButtons, k_enumSize>& GetValues()
	{
		return k_values;
	}

	bool IsAnalog(GamepadButtons a_buttons)
	{
		return a_buttons >= GamepadButtons::kLThumbLeft;
	}
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

const float GamepadState::k_analogAsGamepadButtonThreshold = 0.1f;