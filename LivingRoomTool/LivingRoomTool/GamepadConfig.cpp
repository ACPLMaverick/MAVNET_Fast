#include "GamepadConfig.h"

GamepadConfig::GamepadConfig()
	: Serializable(k_propertyName)
	, m_GUID(L"")
{
	RestoreDefaults();
}

GamepadConfig::GamepadConfig(const std::wstring & a_deviceGUID)
	: Serializable(k_propertyName)
	, m_GUID(a_deviceGUID)
{
	if (LoadFromFile() == false)
	{
		RestoreDefaults();
		SaveToFile();
	}
}

GamepadConfig::~GamepadConfig()
{
}

void GamepadConfig::RestoreDefaults()
{
	static const float k_commonDeadzone = 0.15f;
	static const int32_t k_commonMouseSpeed = 50;
	static const int32_t k_commonMouseScrollSpeed = 3;

	m_deadzoneLeftThumb = k_commonDeadzone;
	m_deadzoneRightThumb = k_commonDeadzone;
	m_deadzoneLeftTrigger = k_commonDeadzone;
	m_deadzoneRightTrigger = k_commonDeadzone;
	m_mouseSpeedX = k_commonMouseSpeed;
	m_mouseSpeedY = k_commonMouseSpeed;
	m_mouseSpeedScroll = k_commonMouseScrollSpeed;

	m_instrumentationMode = InstrumentationMode::kOn;
}

const FilePath GamepadConfig::GetFilePath_Internal()
{
	return FilePath(L"configs", m_GUID);
}

const char* GamepadConfig::k_propertyName = "GamepadConfig";