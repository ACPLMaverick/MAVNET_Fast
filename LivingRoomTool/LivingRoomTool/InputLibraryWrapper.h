#pragma once

#include "Common.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

class InputLibraryWrapper
{
public:

	static void Init();
	static void Cleanup();
	static IDirectInput8* Get();

private:

	static IDirectInput8* s_dinput;
};

