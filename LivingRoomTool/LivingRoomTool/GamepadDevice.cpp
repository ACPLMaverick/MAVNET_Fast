#include "GamepadDevice.h"

#include <wbemidl.h>
#include <oleauto.h>
#include <wbemidl.h>

GamepadDevice::GamepadDevice(
	IDirectInput8* a_dinput,
	LPCDIDEVICEINSTANCE a_device,
	uint32_t possibleXInputIndex)
	: m_state()
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
}

GamepadDevice::~GamepadDevice()
{
}

GamepadDevice::GamepadDevice(const GamepadDevice && a_moved)
	: m_handle(a_moved.m_handle)
	, m_GUID(a_moved.m_GUID)
	, m_name(a_moved.m_name)
	, m_xInputIndex(a_moved.m_xInputIndex)
	, m_state(a_moved.m_state)
{
}

GamepadDevice& GamepadDevice::operator=(const GamepadDevice && a_moved)
{
	m_handle = a_moved.m_handle;
	m_GUID = a_moved.m_GUID;
	m_name = a_moved.m_name;
	m_xInputIndex = a_moved.m_xInputIndex;
	m_state = a_moved.m_state;
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
}

void GamepadDevice::IdentifyByVibrating() const
{
	// TODO Vibrate for a few milliseconds.
}

inline void GamepadDevice::PollStateDirectInput()
{
	// TODO
}

inline void GamepadDevice::PollStateXInput()
{
	// TODO
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