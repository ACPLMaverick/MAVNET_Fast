#include "GamepadConfig.h"

GamepadConfig::GamepadConfig()
	:m_GUID(L"")
{
	CreateDefaultConfig();
}

GamepadConfig::GamepadConfig(const std::wstring & a_deviceGUID)
	: m_GUID(a_deviceGUID)
{
	if (LoadFromFile() == false)
	{
		CreateDefaultConfig();
		SaveToFile();
	}
}

GamepadConfig::~GamepadConfig()
{
}

const Serializable::FilePath GamepadConfig::GetFilePath_Internal()
{
	return FilePath(L"configs", m_GUID);
}

void GamepadConfig::CreateDefaultConfig()
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
