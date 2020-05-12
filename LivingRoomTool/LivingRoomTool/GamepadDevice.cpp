#include "GamepadDevice.h"

#include <wbemidl.h>
#include <oleauto.h>
#include <wbemidl.h>

#include <algorithm>

GamepadDevice::GamepadDevice(
	IDirectInput8* a_dinput,
	LPCDIDEVICEINSTANCE a_device,
	uint32_t possibleXInputIndex)
	: m_state()
	, m_config()
{
	LRT_CheckHR(a_dinput->CreateDevice(a_device->guidInstance, &m_handle, nullptr));
	LRT_Assert(m_handle != nullptr);

	static const size_t k_maxGUIDCharNum = 128;
	m_GUID.resize(k_maxGUIDCharNum);
	int numChars = StringFromGUID2(a_device->guidInstance, const_cast<wchar_t*>(m_GUID.data()), k_maxGUIDCharNum);
	m_GUID.resize(numChars);
	LRT_Assert(m_GUID.size() > 2);
	m_GUID = m_GUID.substr(1, m_GUID.size() - 3);

	m_name = std::wstring(a_device->tszInstanceName);

	m_xInputIndex = IsDInputDeviceAnXInputDevice(a_device->guidProduct) ? possibleXInputIndex : k_invalidXInputIndex;

	// DirectInput device init.
	if (IsXInputDevice() == false)
	{
		InitDirectInput();
	}

	m_config = GamepadConfig(m_GUID);
}

GamepadDevice::~GamepadDevice()
{
	if (m_handle != nullptr)
	{
		if (IsXInputDevice() == false)
		{
			CleanupDirectInput();
		}

		CleanupMembers();
	}
}

GamepadDevice::GamepadDevice(GamepadDevice && a_moved)
	: m_handle(a_moved.m_handle)
	, m_GUID(a_moved.m_GUID)
	, m_name(a_moved.m_name)
	, m_xInputIndex(a_moved.m_xInputIndex)
	, m_state(a_moved.m_state)
	, m_config(a_moved.m_config)
{
	a_moved.CleanupMembers();
}

GamepadDevice& GamepadDevice::operator=(GamepadDevice && a_moved)
{
	m_handle = a_moved.m_handle;
	m_GUID = a_moved.m_GUID;
	m_name = a_moved.m_name;
	m_xInputIndex = a_moved.m_xInputIndex;
	m_state = a_moved.m_state;
	m_config = a_moved.m_config;

	a_moved.CleanupMembers();

	return *this;
}

void GamepadDevice::PollState()
{
	if (IsXInputDevice())
	{
		PollStateXInput();
	}
	else
	{
		PollStateDirectInput();
	}

	ApplyDeadzones();
}

void GamepadDevice::IdentifyByVibrating() const
{
	if (IsXInputDevice())
	{
		VibrateXInput();
	}
	else
	{
		VibrateDirectInput();
	}
}

void GamepadDevice::CleanupMembers()
{
	m_handle = nullptr;
	m_GUID = L"";
	m_name = L"";
	m_xInputIndex = k_invalidXInputIndex;
}

inline void GamepadDevice::InitDirectInput()
{
	// Set cooperative level.
	HWND activeWindow = GetActiveWindow();
	LRT_Assert(activeWindow != 0);
	//LRT_CheckHR(m_handle->SetCooperativeLevel(activeWindow, DISCL_BACKGROUND));	// Fails. Is it even necessary?

	// Set data format.
	LRT_CheckHR(m_handle->SetDataFormat(&c_dfDIJoystick));

	// Use EnumObjects callback to configure device axis.
	LRT_CheckHR(m_handle->EnumObjects(&GamepadDevice::ConfigureDinputDeviceAxis, m_handle, DIDFT_AXIS));

	// Acquire this controller.
	LRT_CheckHR(m_handle->Acquire());
}

inline void GamepadDevice::CleanupDirectInput()
{
	m_handle->Unacquire();
	m_handle->Release();
}

inline void GamepadDevice::PollStateDirectInput()
{
	HRESULT hr = m_handle->Poll();
	if (FAILED(hr))
	{
		// DirectInput lost the device, try to re-acquire it.
		hr = m_handle->Acquire();
		while (hr == DIERR_INPUTLOST)
		{
			hr = m_handle->Acquire();
		}

		// Return if a fatal error is encountered.
		if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED))
		{
			LRT_Fail();
			return;
		}

		// If another application has control of this device, we have to wait for our turn.
		if (hr == DIERR_OTHERAPPHASPRIO)
		{
			return;
		}
	}

	DIJOYSTATE diState{};
	LRT_CheckHR(m_handle->GetDeviceState(sizeof(DIJOYSTATE), &diState));

	// For now I will proceed with interpreting these values for DualShock 4.

#define GET_FLT(_a_) static_cast<float>(_a_) / static_cast<float>(k_directInputAxisRange)
#define GET_FLT_CLAMP01(_a_) max(min(GET_FLT(_a_), 1.0f), 0.0f)
#define GET_FLT_CLAMP11(_a_) max(min(GET_FLT(_a_), 1.0f), -1.0f)

	m_state.m_leftThumb.AxisX = GET_FLT_CLAMP11(diState.lX);
	m_state.m_leftThumb.AxisY = -GET_FLT_CLAMP11(diState.lY);
	m_state.m_rightThumb.AxisX = GET_FLT_CLAMP11(diState.lZ);
	m_state.m_rightThumb.AxisY = -GET_FLT_CLAMP11(diState.lRz);
	m_state.m_leftTrigger.Axis = GET_FLT_CLAMP01(diState.lRx);
	m_state.m_rightTrigger.Axis = GET_FLT_CLAMP01(diState.lRy);

#undef GET_FLT_CLAMP11
#undef GET_FLT_CLAMP01
#undef GET_FLT

	m_state.m_prevButtons = m_state.m_currentButtons;
	m_state.m_currentButtons = GamepadButtons::kNone;

	//  D-Pad
	switch (diState.rgdwPOV[0])
	{
	case 0:
		m_state.m_currentButtons |= GamepadButtons::kLUp;
		break;
	case 4500:
		m_state.m_currentButtons |= GamepadButtons::kLUp | GamepadButtons::kLRight;
		break;
	case 9000:
		m_state.m_currentButtons |= GamepadButtons::kLRight;
		break;
	case 13500:
		m_state.m_currentButtons |= GamepadButtons::kLRight | GamepadButtons::kLDown;
		break;
	case 18000:
		m_state.m_currentButtons |= GamepadButtons::kLDown;
		break;
	case 22500:
		m_state.m_currentButtons |= GamepadButtons::kLDown | GamepadButtons::kLLeft;
		break;
	case 27000:
		m_state.m_currentButtons |= GamepadButtons::kLDown;
		break;
	case 31500:
		m_state.m_currentButtons |= GamepadButtons::kLLeft | GamepadButtons::kLUp;
		break;
	default:
		break;
	}

	// Buttons
	if (m_state.m_leftTrigger.Axis >= GamepadState::k_analogToDigitalThreshold)
	{
		m_state.m_currentButtons |= GamepadButtons::kLT;
	}
	if (m_state.m_rightTrigger.Axis >= GamepadState::k_analogToDigitalThreshold)
	{
		m_state.m_currentButtons |= GamepadButtons::kRT;
	}


	if (diState.rgbButtons[4] != 0)	// L1
	{
		m_state.m_currentButtons |= GamepadButtons::kLB;
	}
	if (diState.rgbButtons[5] != 0)	// R1
	{
		m_state.m_currentButtons |= GamepadButtons::kRB;
	}
	if (diState.rgbButtons[8] != 0)	// Share
	{
		m_state.m_currentButtons |= GamepadButtons::kView;
	}
	if (diState.rgbButtons[9] != 0)	// Options
	{
		m_state.m_currentButtons |= GamepadButtons::kMenu;
	}
	if (diState.rgbButtons[13] != 0)	// Center
	{
		// Not supported.
	}
	if (diState.rgbButtons[12] != 0)	// PS Button
	{
		// Not supported.
	}
	if (diState.rgbButtons[10] != 0)	// L3
	{
		m_state.m_currentButtons |= GamepadButtons::kLThumb;
	}
	if (diState.rgbButtons[11] != 0)	// R3
	{
		m_state.m_currentButtons |= GamepadButtons::kRThumb;
	}
	if (diState.rgbButtons[0] != 0)	// Square
	{
		m_state.m_currentButtons |= GamepadButtons::kRLeft;
	}
	if (diState.rgbButtons[3] != 0)	// Triangle
	{
		m_state.m_currentButtons |= GamepadButtons::kRUp;
	}
	if (diState.rgbButtons[2] != 0)	// Circle
	{
		m_state.m_currentButtons |= GamepadButtons::kRRight;
	}
	if (diState.rgbButtons[1] != 0)	// Cross
	{
		m_state.m_currentButtons |= GamepadButtons::kRDown;
	}
}

inline void GamepadDevice::PollStateXInput()
{
	XINPUT_STATE xiState{};
	XInputGetState(m_xInputIndex, &xiState);
	
	auto funcGetAxis = [](SHORT a_inAxis) -> float
	{
		if (a_inAxis >= 0)
		{
			return static_cast<float>(a_inAxis) / static_cast<float>(SHRT_MAX);
		}
		else
		{
			return -static_cast<float>(a_inAxis) / static_cast<float>(SHRT_MIN);
		}
	};

	auto funcGetTrigger = [](BYTE a_inTrigger) -> float
	{
		return static_cast<float>(a_inTrigger) / static_cast<float>(UINT8_MAX);
	};

	m_state.m_leftThumb.AxisX = funcGetAxis(xiState.Gamepad.sThumbLX);
	m_state.m_leftThumb.AxisY = funcGetAxis(xiState.Gamepad.sThumbLY);
	m_state.m_rightThumb.AxisX = funcGetAxis(xiState.Gamepad.sThumbRX);
	m_state.m_rightThumb.AxisY = funcGetAxis(xiState.Gamepad.sThumbRY);
	m_state.m_leftTrigger.Axis = funcGetTrigger(xiState.Gamepad.bLeftTrigger);
	m_state.m_rightTrigger.Axis = funcGetTrigger(xiState.Gamepad.bRightTrigger);

	m_state.m_prevButtons = m_state.m_currentButtons;
	m_state.m_currentButtons = GamepadButtons::kNone;

	if (m_state.m_leftTrigger.Axis >= GamepadState::k_analogToDigitalThreshold)
	{
		m_state.m_currentButtons |= GamepadButtons::kLT;
	}
	if (m_state.m_rightTrigger.Axis >= GamepadState::k_analogToDigitalThreshold)
	{
		m_state.m_currentButtons |= GamepadButtons::kRT;
	}

	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
	{
		m_state.m_currentButtons |= GamepadButtons::kLUp;
	}
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
	{
		m_state.m_currentButtons |= GamepadButtons::kLDown;
	}
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
	{
		m_state.m_currentButtons |= GamepadButtons::kLLeft;
	}
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
	{
		m_state.m_currentButtons |= GamepadButtons::kLRight;
	}
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_START)
	{
		m_state.m_currentButtons |= GamepadButtons::kMenu;
	}
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
	{
		m_state.m_currentButtons |= GamepadButtons::kView;
	}
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
	{
		m_state.m_currentButtons |= GamepadButtons::kLThumb;
	}
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
	{
		m_state.m_currentButtons |= GamepadButtons::kRThumb;
	}
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
	{
		m_state.m_currentButtons |= GamepadButtons::kLB;
	}
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
	{
		m_state.m_currentButtons |= GamepadButtons::kRB;
	}
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_A)
	{
		m_state.m_currentButtons |= GamepadButtons::kRDown;
	}
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_B)
	{
		m_state.m_currentButtons |= GamepadButtons::kRRight;
	}
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_X)
	{
		m_state.m_currentButtons |= GamepadButtons::kRLeft;
	}
	if (xiState.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
	{
		m_state.m_currentButtons |= GamepadButtons::kRUp;
	}
}

inline void GamepadDevice::ApplyDeadzones()
{
	//LRT_Todo();
}

inline void GamepadDevice::VibrateDirectInput() const
{
	DIEFFECT effect = {};
	DWORD axes[] = { DIJOFS_X, DIJOFS_Y };
	LONG directions[2] = { 18000, 0 };
	
	DICONSTANTFORCE constantForce;
	constantForce.lMagnitude = DI_FFNOMINALMAX;

	effect.dwSize = sizeof(DIEFFECT);
	effect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
	effect.dwDuration = (DWORD)(0.5 * DI_SECONDS);
	effect.dwSamplePeriod = 0;
	effect.dwGain = DI_FFNOMINALMAX;
	effect.dwTriggerButton = DIEB_NOTRIGGER;
	effect.dwTriggerRepeatInterval = 0;
	effect.cAxes = 2;
	effect.rgdwAxes = axes;
	effect.rglDirection = directions;
	effect.lpEnvelope = nullptr;
	effect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
	effect.lpvTypeSpecificParams = &constantForce;

	LPDIRECTINPUTEFFECT lpdiEffect = {};
	HRESULT hr = m_handle->CreateEffect(GUID_ConstantForce, &effect, &lpdiEffect, NULL);
	if (hr != S_OK)
	{
		LRT_PrintHResult(hr);
		return;
	}

	LRT_CheckHR(lpdiEffect->Start(1, 0));

	Sleep(500);

	lpdiEffect->Release();
}

inline void GamepadDevice::VibrateXInput() const
{
	static XINPUT_VIBRATION k_vibrationOn = { UINT16_MAX, 0 };
	static XINPUT_VIBRATION k_vibrationOff = { 0, 0 };
	
	LRT_Assert(XInputSetState(m_xInputIndex, &k_vibrationOn) == ERROR_SUCCESS);

	Sleep(500);

	LRT_Assert(XInputSetState(m_xInputIndex, &k_vibrationOff) == ERROR_SUCCESS);
}

void GamepadDevice::PrintDinputState(const DIJOYSTATE & a_diState)
{
	wchar_t buffer[4096];
	swprintf_s(buffer, 4096,
		L"=========================================\n"
		L"DirectInput state:\n"
		L"lX : [%d], lY : [%d], lZ : [%d]\n"
		L"lRx : [%d], lRy : [%d], lRz : [%d]\n"
		L"rglSlider[0] : [%d], rglSlider[1] : [%d]\n"
		L"rgdwPOV[0] : [%d], rgdwPOV[1] : [%d], rgdwPOV[2] : [%d], rgdwPOV[3] : [%d]\n"
		L"rgbButtons[0] : [%d]\n"
		L"rgbButtons[1] : [%d]\n"
		L"rgbButtons[2] : [%d]\n"
		L"rgbButtons[3] : [%d]\n"
		L"rgbButtons[4] : [%d]\n"
		L"rgbButtons[5] : [%d]\n"
		L"rgbButtons[6] : [%d]\n"
		L"rgbButtons[7] : [%d]\n"
		L"rgbButtons[8] : [%d]\n"
		L"rgbButtons[9] : [%d]\n"
		L"rgbButtons[10] : [%d]\n"
		L"rgbButtons[11] : [%d]\n"
		L"rgbButtons[12] : [%d]\n"
		L"rgbButtons[13] : [%d]\n"
		L"rgbButtons[14] : [%d]\n"
		L"rgbButtons[15] : [%d]\n"
		L"rgbButtons[16] : [%d]\n"
		L"rgbButtons[17] : [%d]\n"
		L"rgbButtons[18] : [%d]\n"
		L"rgbButtons[19] : [%d]\n"
		L"rgbButtons[20] : [%d]\n"
		L"rgbButtons[21] : [%d]\n"
		L"rgbButtons[22] : [%d]\n"
		L"rgbButtons[23] : [%d]\n"
		L"rgbButtons[24] : [%d]\n"
		L"rgbButtons[25] : [%d]\n"
		L"rgbButtons[26] : [%d]\n"
		L"rgbButtons[27] : [%d]\n"
		L"rgbButtons[28] : [%d]\n"
		L"rgbButtons[29] : [%d]\n"
		L"rgbButtons[30] : [%d]\n"
		L"rgbButtons[31] : [%d]\n"
		L"=========================================\n",
		a_diState.lX, a_diState.lY, a_diState.lZ,
		a_diState.lRx, a_diState.lRy, a_diState.lRz, a_diState.rglSlider[0], a_diState.rglSlider[1],
		a_diState.rgdwPOV[0], a_diState.rgdwPOV[1], a_diState.rgdwPOV[2], a_diState.rgdwPOV[3],
		a_diState.rgbButtons[0], a_diState.rgbButtons[1], a_diState.rgbButtons[2],
		a_diState.rgbButtons[3], a_diState.rgbButtons[4], a_diState.rgbButtons[5],
		a_diState.rgbButtons[6], a_diState.rgbButtons[7], a_diState.rgbButtons[8],
		a_diState.rgbButtons[9], a_diState.rgbButtons[10], a_diState.rgbButtons[11],
		a_diState.rgbButtons[12], a_diState.rgbButtons[13], a_diState.rgbButtons[14],
		a_diState.rgbButtons[15], a_diState.rgbButtons[16], a_diState.rgbButtons[17],
		a_diState.rgbButtons[18], a_diState.rgbButtons[19], a_diState.rgbButtons[20],
		a_diState.rgbButtons[21], a_diState.rgbButtons[22], a_diState.rgbButtons[23],
		a_diState.rgbButtons[24], a_diState.rgbButtons[25], a_diState.rgbButtons[26],
		a_diState.rgbButtons[27], a_diState.rgbButtons[28], a_diState.rgbButtons[29],
		a_diState.rgbButtons[30], a_diState.rgbButtons[31]);
	OutputDebugString(buffer);
}

void GamepadDevice::PrintGamepadState(const GamepadState & state)
{
	wchar_t buffer[4096];
	swprintf_s(buffer, 4096,
		L"=========================================\n"
		L"GamepadState:\n"
		L"LX [%f] LY [%f] RX [%f] RY [%f] LT [%f] RT [%f]\n"
		L"CurrentButtons [%16x]\n"
		L"PrevButtons    [%16x]\n"
		L"=========================================\n",
		state.m_leftThumb.AxisX, state.m_leftThumb.AxisY,
		state.m_rightThumb.AxisX, state.m_rightThumb.AxisY,
		state.m_leftTrigger.Axis, state.m_rightTrigger.Axis,
		state.m_currentButtons, state.m_prevButtons
	);

	OutputDebugString(buffer);
}

// Got this from here : https://docs.microsoft.com/en-us/windows/win32/xinput/xinput-and-directinput
//-----------------------------------------------------------------------------
// Enum each PNP device using WMI and check each device ID to see if it contains 
// "IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it's an XInput device
// Unfortunately this information can not be found by just using DirectInput 
//-----------------------------------------------------------------------------
bool GamepadDevice::IsDInputDeviceAnXInputDevice(const GUID & a_guidProductFromDirectInput)
{
	IWbemLocator*           pIWbemLocator = NULL;
	IEnumWbemClassObject*   pEnumDevices = NULL;
	IWbemClassObject*       pDevices[20] = { 0 };
	IWbemServices*          pIWbemServices = NULL;
	BSTR                    bstrNamespace = NULL;
	BSTR                    bstrDeviceID = NULL;
	BSTR                    bstrClassName = NULL;
	DWORD                   uReturned = 0;
	bool                    bIsXinputDevice = false;
	UINT                    iDevice = 0;
	VARIANT                 var;
	HRESULT                 hr;

	// CoInit if needed
	hr = CoInitialize(NULL);
	bool bCleanupCOM = SUCCEEDED(hr);

	// Create WMI
	hr = CoCreateInstance(__uuidof(WbemLocator),
		NULL,
		CLSCTX_INPROC_SERVER,
		__uuidof(IWbemLocator),
		(LPVOID*)&pIWbemLocator);
	if (FAILED(hr) || pIWbemLocator == NULL)
		goto LCleanup;

	bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2"); if (bstrNamespace == NULL) goto LCleanup;
	bstrClassName = SysAllocString(L"Win32_PNPEntity");   if (bstrClassName == NULL) goto LCleanup;
	bstrDeviceID = SysAllocString(L"DeviceID");          if (bstrDeviceID == NULL)  goto LCleanup;

	// Connect to WMI 
	hr = pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, 0L,
		0L, NULL, NULL, &pIWbemServices);
	if (FAILED(hr) || pIWbemServices == NULL)
		goto LCleanup;

	// Switch security level to IMPERSONATE. 
	CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
		RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

	hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, NULL, &pEnumDevices);
	if (FAILED(hr) || pEnumDevices == NULL)
		goto LCleanup;

	// Loop over all devices
	for (;; )
	{
		// Get 20 at a time
		hr = pEnumDevices->Next(10000, 20, pDevices, &uReturned);
		if (FAILED(hr))
			goto LCleanup;
		if (uReturned == 0)
			break;

		for (iDevice = 0; iDevice < uReturned; iDevice++)
		{
			// For each device, get its device ID
			hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, NULL, NULL);
			if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL)
			{
				// Check if the device ID contains "IG_".  If it does, then it's an XInput device
					// This information can not be found from DirectInput 
				if (wcsstr(var.bstrVal, L"IG_"))
				{
					// If it does, then get the VID/PID from var.bstrVal
					DWORD dwPid = 0, dwVid = 0;
					WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
					if (strVid && swscanf(strVid, L"VID_%4X", &dwVid) != 1)
						dwVid = 0;
					WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
					if (strPid && swscanf(strPid, L"PID_%4X", &dwPid) != 1)
						dwPid = 0;

					// Compare the VID/PID to the DInput device
					DWORD dwVidPid = MAKELONG(dwVid, dwPid);
					if (dwVidPid == a_guidProductFromDirectInput.Data1)
					{
						bIsXinputDevice = true;
						goto LCleanup;
					}
				}
			}
			pDevices[iDevice]->Release();
			pDevices[iDevice] = nullptr;
		}
	}

LCleanup:
	if (bstrNamespace)
		SysFreeString(bstrNamespace);
	if (bstrDeviceID)
		SysFreeString(bstrDeviceID);
	if (bstrClassName)
		SysFreeString(bstrClassName);
	for (iDevice = 0; iDevice < 20; iDevice++)
	{
		if (pDevices[iDevice] != nullptr)
		{
			pDevices[iDevice]->Release();
			pDevices[iDevice] = nullptr;
		}
	}
	if (pEnumDevices != nullptr)
		pEnumDevices->Release();
	if (pIWbemLocator != nullptr)
		pIWbemLocator->Release();
	if (pIWbemServices != nullptr)
		pIWbemServices->Release();

	if (bCleanupCOM)
		CoUninitialize();

	return bIsXinputDevice;
}

BOOL GamepadDevice::ConfigureDinputDeviceAxis(LPCDIDEVICEOBJECTINSTANCEW a_instance, LPVOID a_param)
{
	// Set axis value range.
	LPDIRECTINPUTDEVICE8 gameController = (LPDIRECTINPUTDEVICE8)a_param;
	gameController->Unacquire();

	// Structure to hold game controller range properties.
	DIPROPRANGE gameControllerRange;

	// Set the range.
	gameControllerRange.lMin = -k_directInputAxisRange;
	gameControllerRange.lMax = k_directInputAxisRange;

	// Set the size of the structure.
	gameControllerRange.diph.dwSize = sizeof(DIPROPRANGE);
	gameControllerRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);

	// Set the object that we want to change.
	gameControllerRange.diph.dwHow = DIPH_BYID;
	gameControllerRange.diph.dwObj = a_instance->dwType;

	if (FAILED(gameController->SetProperty(DIPROP_RANGE, &gameControllerRange.diph))) 
	{
		LRT_Fail();
		return DIENUM_STOP;
	}


	// Structure to hold game controller axis dead zone.
	DIPROPDWORD gameControllerDeadZone;

	// Set the dead zone to 0. Deadzone will be resolved programmaticaly.
	gameControllerDeadZone.dwData = 0;

	// Set the size of the structure.
	gameControllerDeadZone.diph.dwSize = sizeof(DIPROPDWORD);
	gameControllerDeadZone.diph.dwHeaderSize = sizeof(DIPROPHEADER);

	// Set the object that we want to change.
	gameControllerDeadZone.diph.dwHow = DIPH_BYID;
	gameControllerDeadZone.diph.dwObj = a_instance->dwType;

	// Now set the dead zone for the axis.
	if (FAILED(gameController->SetProperty(DIPROP_DEADZONE, &gameControllerDeadZone.diph)))
	{
		LRT_Fail();
		return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

const uint32_t GamepadDevice::k_invalidXInputIndex = static_cast<uint32_t>(-1); 
const uint32_t GamepadDevice::k_invalidDeviceIndex = static_cast<uint32_t>(-1);
const int16_t GamepadDevice::k_directInputAxisRange = INT16_MAX;
const float GamepadDevice::k_identifyVibrationPower = 0.67f;
const int32_t GamepadDevice::k_identifyVibrationTimeMs = 500;