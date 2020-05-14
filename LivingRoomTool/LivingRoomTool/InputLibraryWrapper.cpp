#include "InputLibraryWrapper.h"

void InputLibraryWrapper::Init()
{
	LRT_CheckHR(DirectInput8Create
	(
		GetModuleHandle(NULL),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<LPVOID*>(&s_dinput),
		NULL
	));
	LRT_Assert(s_dinput != nullptr);
}

void InputLibraryWrapper::Cleanup()
{
	if (s_dinput != nullptr)
	{
		s_dinput->Release();
		s_dinput = nullptr;
	}
}

IDirectInput8* InputLibraryWrapper::Get()
{
	LRT_Assert(s_dinput != nullptr);
	return s_dinput;
}

IDirectInput8* InputLibraryWrapper::s_dinput = nullptr;
