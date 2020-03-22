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
	static const float k_commonDeadzone = 0.1f;
	static const float k_commonMouseSpeed = 0.5f;

	m_deadzoneLeftThumb = k_commonDeadzone;
	m_deadzoneRightThumb = k_commonDeadzone;
	m_deadzoneLeftTrigger = k_commonDeadzone;
	m_deadzoneRightTrigger = k_commonDeadzone;
	m_mouseSpeedX = k_commonMouseSpeed;
	m_mouseSpeedY = k_commonMouseSpeed;

	m_instrumentationMode = InstrumentationMode::kOn;
}

const FilePath GamepadConfig::GetFilePath_Internal()
{
	return FilePath(L"configs", m_GUID);
}

const char* GamepadConfig::k_propertyName = "GamepadConfig";