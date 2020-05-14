#include "GamepadProcessor.h"

#include "InputLibraryWrapper.h"

#define LRT_CheckDeviceIndex(_index_, _toReturn_)	\
if (_index_ >= m_devices.size())					\
{													\
	LRT_Fail();										\
	return _toReturn_;								\
}

GamepadProcessor::GamepadProcessor()
	: BaseProcessor()
	, m_HACK_xInputDeviceCounter(0)
{
}

GamepadProcessor::~GamepadProcessor()
{
}

void GamepadProcessor::RescanGamepads()
{
	m_devices.clear();
	m_HACK_xInputDeviceCounter = 0;

	LRT_CheckHR(InputLibraryWrapper::Get()->EnumDevices(
		DI8DEVCLASS_GAMECTRL,
		StaticDinputEnumerateGamepads,
		reinterpret_cast<LPVOID>(this),
		DIEDFL_ATTACHEDONLY
	));
}

void GamepadProcessor::PollInputStates()
{
	for (GamepadDevice& device : m_devices)
	{
		device.PollState();
	}
}

GamepadDevice & GamepadProcessor::GetGamepadDevice(size_t a_index)
{
	LRT_CheckDeviceIndex(a_index, m_devices[0]);
	return m_devices[a_index];
}

const GamepadDevice & GamepadProcessor::GetGamepadDevice(size_t a_index) const
{
	LRT_CheckDeviceIndex(a_index, m_devices[0]);
	return m_devices[a_index];
}

size_t GamepadProcessor::GetDevicesNum() const
{
	return m_devices.size();
}

void GamepadProcessor::GetDeviceNames(std::vector<std::wstring>& a_outLabels) const
{
	a_outLabels.clear();

	for (const GamepadDevice& device : m_devices)
	{
		a_outLabels.push_back(device.GetName());
	}
}

void GamepadProcessor::IdentifyDeviceByVibrating(size_t a_index) const
{
	LRT_CheckDeviceIndex(a_index, void());
	m_devices[a_index].IdentifyByVibrating();
}

void GamepadProcessor::Init_Internal()
{
	// Create DirectInput main handle.
	InputLibraryWrapper::Init();
}

void GamepadProcessor::Cleanup_Internal()
{
	m_devices.clear();
	m_HACK_xInputDeviceCounter = 0;

	InputLibraryWrapper::Cleanup();
}

BOOL GamepadProcessor::StaticDinputEnumerateGamepads(LPCDIDEVICEINSTANCE a_lpddi, LPVOID a_pvRef)
{
	if (a_pvRef == nullptr)
	{
		LRT_Fail();
		return DIENUM_STOP;
	}

	if (a_lpddi == nullptr)
	{
		LRT_Fail();
		return DIENUM_CONTINUE;
	}

	auto* instance = reinterpret_cast<GamepadProcessor*>(a_pvRef);
	return instance->DinputEnumerateGamepads(a_lpddi);
}

BOOL GamepadProcessor::DinputEnumerateGamepads(LPCDIDEVICEINSTANCE a_device)
{
	m_devices.push_back(std::move(GamepadDevice(InputLibraryWrapper::Get(), a_device, m_HACK_xInputDeviceCounter)));

	if (m_devices.back().IsXInputDevice())
	{
		++m_HACK_xInputDeviceCounter;
	}

	return DIENUM_CONTINUE;
}