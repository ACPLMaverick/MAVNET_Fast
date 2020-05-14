#pragma once

#include "Common.h"

enum class InputActionKey : uint8_t;

/**
	It is responsible for converting between InputActionKey and platform keys.
*/
class InputActionConverter
{
public:

	using PlatformKey = uint8_t;

	PlatformKey ToPlatformKey(InputActionKey key);
	InputActionKey FromPlatformKey(PlatformKey platformKey);

private:

	static InputActionKey* GetPlatformKeyToAction();

	static const PlatformKey k_actionKeyToPlatform[];
};

