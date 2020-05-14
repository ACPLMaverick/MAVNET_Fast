#include "InputActionKeypressRetriever.h"

#include "InputAction.h"

InputActionKeypressRetriever::InputActionKeypressRetriever()
	: m_keyboard(nullptr)
	, m_mouse(nullptr)
	, m_bPressStarted(false)
{
}

InputActionKeypressRetriever::~InputActionKeypressRetriever()
{
	LRT_Assert(IsInit() == false);
}

void InputActionKeypressRetriever::Init()
{
	if (IsInit())
	{
		return;
	}

	IDirectInput8* dinput = InputLibraryWrapper::Get();
	//HWND activeWindow = GetActiveWindow();
	
	LRT_CheckHR(dinput->CreateDevice(GUID_SysKeyboard, &m_keyboard, nullptr));
	LRT_CheckHR(m_keyboard->SetDataFormat(&c_dfDIKeyboard));
	//LRT_CheckHR(m_keyboard->SetCooperativeLevel(activeWindow, DISCL_FOREGROUND | DISCL_EXCLUSIVE));
	LRT_CheckHR(m_keyboard->Acquire());

	LRT_CheckHR(dinput->CreateDevice(GUID_SysMouse, &m_mouse, nullptr));
	LRT_CheckHR(m_mouse->SetDataFormat(&c_dfDIMouse));
	//LRT_CheckHR(m_mouse->SetCooperativeLevel(activeWindow, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	LRT_CheckHR(m_mouse->Acquire());

	m_pressed.clear();
	ZeroMemory(m_keyboardState, sizeof(m_keyboardState));
	ZeroMemory(&m_mouseState, sizeof(m_mouseState));
	m_bPressStarted = false;
}

bool InputActionKeypressRetriever::Tick()
{
	if (IsInit() == false)
	{
		return false;
	}

	ReadKeyboard();
	ReadMouse();
	FindPressed();

	return IsPressEnded();
}

void InputActionKeypressRetriever::RetrieveAndCleanup(std::vector<InputActionKey>& a_outKeypresses)
{
	LRT_Assert(IsInit());

	for (InputActionKey key : m_pressed)
	{
		a_outKeypresses.push_back(key);
	}

	Cleanup();
}

InputActionKey InputActionKeypressRetriever::RetrieveAndCleanup()
{
	LRT_Assert(IsInit());

	const InputActionKey keyToReturn = m_pressed.empty() ? InputActionKey::kNone : *m_pressed.begin();

	Cleanup();
	return keyToReturn;
}

inline void InputActionKeypressRetriever::ReadKeyboard()
{
	HRESULT result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		LRT_Fail();

		// If the keyboard lost focus try to get it back.
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			m_keyboard->Acquire();
		}
	}
}

inline void InputActionKeypressRetriever::ReadMouse()
{
	HRESULT result = m_mouse->GetDeviceState(sizeof(m_mouseState), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		LRT_Fail();

		// If the keyboard lost focus try to get it back.
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			m_mouse->Acquire();
		}
	}
}

inline void InputActionKeypressRetriever::FindPressed()
{
	for (size_t i = 0; i < 256; ++i)
	{
		if (m_keyboardState[i] & 0x80)
		{
			InputActionKey key = m_converter.FromPlatformKey(static_cast<InputActionConverter::PlatformKey>(i));
			if (key != InputActionKey::kNone && key < InputActionKey::ENUM_SIZE)
			{
				m_pressed.insert(key);
			}
		}
	}

	if (m_mouseState.rgbButtons[0])
	{
		m_pressed.insert(InputActionKey::kMouseLMB);
	}
	if (m_mouseState.rgbButtons[1])
	{
		m_pressed.insert(InputActionKey::kMouseRMB);
	}
	if (m_mouseState.rgbButtons[2])
	{
		m_pressed.insert(InputActionKey::kMouseMMB);
	}

	if (m_bPressStarted == false && m_pressed.size() > 0)
	{
		m_bPressStarted = true;
	}
}

inline bool InputActionKeypressRetriever::IsPressEnded()
{
	static const uint8_t k_testKeyboardBlock[256] = {};
	static const BYTE k_testMouseBlock[4] = {};

	return m_bPressStarted
		&& memcmp(m_keyboardState, k_testKeyboardBlock, sizeof(m_keyboardState)) == 0
		&& memcmp(m_mouseState.rgbButtons, k_testMouseBlock, sizeof(m_mouseState.rgbButtons)) == 0;
}

void InputActionKeypressRetriever::Cleanup()
{
	if (IsInit() == false)
	{
		return;
	}

	m_keyboard->Unacquire();
	m_keyboard->Release();
	m_keyboard = nullptr;

	m_mouse->Unacquire();
	m_mouse->Release();
	m_mouse = nullptr;
}
