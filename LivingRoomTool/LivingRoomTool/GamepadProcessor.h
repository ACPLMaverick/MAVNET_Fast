#pragma once
#include "BaseProcessor.h"

#include <Windows.h>
#include <Xinput.h>
#pragma comment (lib, "Xinput9_1_0.lib")

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

class GamepadProcessor :
	public BaseProcessor
{
public:

	enum Buttons
	{
		Button_None			= 0,
		Button_LT			= 1 << 0,
		Button_LB			= 1 << 1,
		Button_LThumb		= 1 << 2,
		Button_LLeft		= 1 << 3,
		Button_LUp			= 1 << 4,
		Button_LRight		= 1 << 5,
		Button_LDown		= 1 << 6,
		Button_View			= 1 << 7,
		Button_Menu			= 1 << 8,
		Button_RT			= 1 << 9,
		Button_RB			= 1 << 10,
		Button_RThumb		= 1 << 11,
		Button_RLeft		= 1 << 12,
		Button_RUp			= 1 << 13,
		Button_RRight		= 1 << 14,
		Button_RDown		= 1 << 15
	};

	struct Thumb
	{
		float AxisX;
		float AxisY;
	};

	struct Trigger
	{
		float Axis;
	};

	class InputState
	{
	public:

		InputState();

		InputState(const InputState&) = default;
		InputState& operator=(const InputState&) = default;

		bool AreButtonsHeld(Buttons buttons) const;
		bool AreButtonsPressed(Buttons buttons) const;
		bool AreButtonsReleased(Buttons buttons) const;

		const Thumb& GetLeftThumb() const { return m_leftThumb; }
		const Thumb& GetRightThumb() const { return m_rightThumb; }
		const Trigger& GetLeftTrigger() const { return m_leftTrigger; }
		const Trigger& GetRightTrigger() const { return m_rightTrigger; }

	private:

		Buttons m_currentButtons;
		Buttons m_prevButtons;

		Thumb m_leftThumb;
		Thumb m_rightThumb;
		Trigger m_leftTrigger;
		Trigger m_rightTrigger;
	};

protected:

	class Device
	{
	public:
		using InternalHandle = IDirectInputDevice8*;

		explicit Device
		(
			IDirectInput8* dinput, 
			LPCDIDEVICEINSTANCE device, 
			uint32_t possibleXInputIndex
		);
		~Device();

		LRT_DisallowCopy(Device);

		Device(const Device&&);
		Device& operator=(const Device&&);

		InputState& PollInputState();
		void IdentifyByVibrating() const;
		
		bool IsXInputDevice() const { return m_xInputIndex != k_invalidDeviceIndex; }
		const std::wstring& GetName() const { return m_name; }

	private:

		inline void PollInputStateDirectInput();
		inline void PollInputStateXInput();

		static bool IsDInputDeviceAnXInputDevice(const GUID& guidProductFromDirectInput);

		InternalHandle m_handle;
		std::wstring m_GUID;
		std::wstring m_name;
		uint32_t m_xInputIndex;

		InputState m_inputState;

		// TODO Config 

		static const uint32_t k_invalidXInputIndex = static_cast<uint32_t>(-1);
	};

public:

	GamepadProcessor();
	~GamepadProcessor();

	LRT_DisallowCopy(GamepadProcessor);

	void FindGamepads();
	const InputState* PollInputState();
	void GetDeviceNames(std::vector<std::wstring>& outLabels) const;
	void SelectDevice(uint32_t deviceIndex);
	void DeselectDevice();
	void IdentifyDeviceByVibrating(uint32_t deviceIndex) const;

protected:

	// Inherited via BaseProcessor
	virtual void Init_Internal() override;
	virtual void Cleanup_Internal() override;

	static BOOL StaticDinputEnumerateGamepads(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
	BOOL DinputEnumerateGamepads(LPCDIDEVICEINSTANCE device);


	std::vector<Device> m_devices;
	uint32_t m_selectedDevice;
	uint32_t m_HACK_xInputDeviceCounter;

	IDirectInput8* m_dinput;

	static const uint32_t k_invalidDeviceIndex = static_cast<uint32_t>(-1);
};

