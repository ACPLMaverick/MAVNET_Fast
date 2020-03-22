#pragma once

#include "Common.h"
#include "Serializable.h"

class GamepadConfig : public Serializable
{
public:

	enum class InstrumentationMode : uint8_t
	{
		kOff,
		// Gamepad is instrumented only with simplified desktop mode preset and it does not use any InputPreset. In game, its instrumentation is disabled completely.
		kDesktop,
		// Gamepad is instrumented with its selected InputPreset only in game. In desktop, it uses simplified desktop mode preset.
		kGame,
		kOn,

		ENUM_SIZE
	};

	GamepadConfig();
	GamepadConfig(const std::wstring& deviceGUID);
	virtual ~GamepadConfig();

	void RestoreDefaults();

private:

	const FilePath GetFilePath_Internal() override;

	std::wstring m_GUID;

	LRT_PROPERTY(GamepadConfig, float, deadzoneLeftThumb, 0.0f);
	LRT_PROPERTY(GamepadConfig, float, deadzoneRightThumb, 0.0f);
	LRT_PROPERTY(GamepadConfig, float, deadzoneLeftTrigger, 0.0f);
	LRT_PROPERTY(GamepadConfig, float, deadzoneRightTrigger, 0.0f);
	LRT_PROPERTY(GamepadConfig, float, mouseSpeedX, 0.0f);
	LRT_PROPERTY(GamepadConfig, float, mouseSpeedY, 0.0f);
	LRT_PROPERTY(GamepadConfig, InstrumentationMode, instrumentationMode, InstrumentationMode::kOn);

	static const char* k_propertyName;
};

