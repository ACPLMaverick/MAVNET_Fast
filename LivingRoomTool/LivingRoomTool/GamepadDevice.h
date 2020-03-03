#pragma once

#include "Common.h"

#include "GamepadState.h"

#include <Windows.h>
#include <Xinput.h>
#pragma comment (lib, "Xinput9_1_0.lib")

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

class GamepadDevice
{
public:
	using InternalHandle = IDirectInputDevice8 * ;

	explicit GamepadDevice
	(
		IDirectInput8* dinput,
		LPCDIDEVICEINSTANCE device,
		uint32_t possibleXInputIndex
	);
	~GamepadDevice();

	LRT_DisallowCopy(GamepadDevice);

	GamepadDevice(const GamepadDevice&&);
	GamepadDevice& operator=(const GamepadDevice&&);

	void PollState();
	const GamepadState& GetState() const { return m_state; }

	void IdentifyByVibrating() const;

	bool IsXInputDevice() const { return m_xInputIndex != k_invalidDeviceIndex; }
	const std::wstring& GetName() const { return m_name; }

private:

	inline void PollStateDirectInput();
	inline void PollStateXInput();

	static bool IsDInputDeviceAnXInputDevice(const GUID& guidProductFromDirectInput);

	InternalHandle m_handle;
	std::wstring m_GUID;
	std::wstring m_name;
	uint32_t m_xInputIndex;

	GamepadState m_state;

	// TODO Config 

	static const uint32_t k_invalidXInputIndex = static_cast<uint32_t>(-1);
	static const uint32_t k_invalidDeviceIndex = static_cast<uint32_t>(-1);
};

