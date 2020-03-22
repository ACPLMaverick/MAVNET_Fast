#include "InputBinding.h"



InputBinding::InputBinding()
	: Serializable(k_propertyName)
{
}

InputBinding::InputBinding(GamepadButtons source, InputActionKey destination)
	: Serializable(k_propertyName)
{
	m_sources.Get().push_back(source);
	m_destinations.Get().push_back(destination);
}

InputBinding::InputBinding(const Sources & sources, const Destinations & destinations)
	: Serializable(k_propertyName)
{
	m_sources.Get() = sources;
	m_destinations.Get() = destinations;
}

InputBinding::~InputBinding()
{
}

void InputBinding::GenerateActions(const GamepadState & gamepadState, const GamepadConfig & gamepadConfig, std::vector<InputAction>& outActions) const
{
	LRT_Todo();
}

const FilePath InputBinding::GetFilePath_Internal()
{
	return FilePath(L"", L"inputBinding");
}

const char* InputBinding::k_propertyName = "InputBinding";
const int32_t InputBinding::k_mouseMovementMultiplier = 20;
const int32_t k_mouseScrollMultiplier = 5;