#pragma once
#include "BaseProcessor.h"
#include "GamepadDevice.h"

class GamepadProcessor :
	public BaseProcessor
{
public:

	GamepadProcessor();
	~GamepadProcessor();

	LRT_DisallowCopy(GamepadProcessor);

	void RescanGamepads();
	void PollInputStates();
	const GamepadDevice& GetGamepadDevice(size_t index) const;
	size_t GetDevicesNum() const;
	void GetDeviceNames(std::vector<std::wstring>& outLabels) const;
	void IdentifyDeviceByVibrating(size_t index) const;

protected:

	// Inherited via BaseProcessor
	virtual void Init_Internal() override;
	virtual void Cleanup_Internal() override;

	static BOOL StaticDinputEnumerateGamepads(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
	BOOL DinputEnumerateGamepads(LPCDIDEVICEINSTANCE device);


	std::vector<GamepadDevice> m_devices;
	size_t m_HACK_xInputDeviceCounter;

	IDirectInput8* m_dinput;
};

