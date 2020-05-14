#pragma once

#include "Common.h"

#include "GamepadState.h"
#include "GamepadConfig.h"

#include <Windows.h>
#include <Xinput.h>
#pragma comment (lib, "Xinput9_1_0.lib")

#include "InputLibraryWrapper.h"

class GamepadDevice
{
public:
	using InternalHandle = IDirectInputDevice8* ;

	explicit GamepadDevice
	(
		IDirectInput8* dinput,
		LPCDIDEVICEINSTANCE device,
		uint32_t possibleXInputIndex
	);
	~GamepadDevice();

	LRT_DisallowCopy(GamepadDevice);

	GamepadDevice(GamepadDevice&&);
	GamepadDevice& operator=(GamepadDevice&&);

	void PollState();
	const GamepadState& GetState() const { return m_state; }
	GamepadConfig& GetConfig() { return m_config; }
	const GamepadConfig& GetConfig() const { return m_config; }

	void IdentifyByVibrating() const;

	bool IsXInputDevice() const { return m_xInputIndex != k_invalidDeviceIndex; }
	const std::wstring& GetName() const { return m_name; }

private:

	void CleanupMembers();

	inline void InitDirectInput();
	inline void CleanupDirectInput();

	inline void PollStateDirectInput();
	inline void PollStateXInput();
				
	inline void ApplyDeadzones();

	inline void VibrateDirectInput() const;
	inline void VibrateXInput() const;

	static void PrintDinputState(const DIJOYSTATE& diState);
	static void PrintGamepadState(const GamepadState& state);
	static bool IsDInputDeviceAnXInputDevice(const GUID& guidProductFromDirectInput);
	static BOOL CALLBACK ConfigureDinputDeviceAxis(LPCDIDEVICEOBJECTINSTANCEW instance, LPVOID param);

	InternalHandle m_handle;
	std::wstring m_GUID;
	std::wstring m_name;
	uint32_t m_xInputIndex;

	GamepadState m_state;
	GamepadConfig m_config;

	static const uint32_t k_invalidXInputIndex;
	static const uint32_t k_invalidDeviceIndex;
	static const int16_t k_directInputAxisRange;
	static const float k_identifyVibrationPower;
	static const int32_t k_identifyVibrationTimeMs;
};

