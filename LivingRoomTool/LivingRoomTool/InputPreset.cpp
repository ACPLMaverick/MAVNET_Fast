#include "InputPreset.h"
#include "InputPresetManager.h"

InputPreset::InputPreset(const char * name)
	: Serializable(name)
{
	LRT_Verify(LoadFromFile());
}

InputPreset::InputPreset(InputPreset && a_move)
	: Serializable(a_move.m_name.c_str())
{
	for (InputBinding* binding : a_move.m_bindings.Get())
	{
		m_bindings.Get().push_back(binding);
	}
	a_move.m_bindings.Get().clear();
}

InputPreset::~InputPreset()
{
}

void InputPreset::GenerateActions(const GamepadState & gamepadState, const GamepadConfig & gamepadConfig, std::vector<InputAction>& outActions) const
{
	const std::vector<InputBinding*>& bindings = m_bindings.Get();
	for (const InputBinding* binding : bindings)
	{
		binding->GenerateActions(gamepadState, gamepadConfig, outActions);
	}
}

void InputPreset::Rename(const std::string& newName)
{
	m_name = newName;
}

InputPreset InputPreset::CreateDefault()
{
	InputPreset defaultPreset("default");

	defaultPreset.Get_bindings() =
	{
		new InputBinding(GamepadButtons::kRDown, InputActionKey::kEnter),
		new InputBinding(GamepadButtons::kRRight, InputActionKey::kEsc),
		new InputBinding(GamepadButtons::kRLeft, InputActionKey::kMouseLMB),
		new InputBinding(GamepadButtons::kRUp, InputActionKey::kMouseRMB),

		new InputBinding(GamepadButtons::kLThumb, InputActionKey::kTab),
		new InputBinding(GamepadButtons::kRThumb, InputActionKey::kSystemKey),

		new InputBinding(GamepadButtons::kView, InputActionKey::kPrintScreen),
		new InputBinding({GamepadButtons::kMenu}, {InputActionKey::kLeftAlt, InputActionKey::kF4}),

		new InputBinding({GamepadButtons::kLB}, {InputActionKey::kLeftAlt, InputActionKey::kArrowLeft}),
		new InputBinding({GamepadButtons::kRB}, {InputActionKey::kLeftAlt, InputActionKey::kArrowRight}),

		new InputBinding(GamepadButtons::kLThumb, InputActionKey::kMouseScrollDown),
		new InputBinding(GamepadButtons::kRThumb, InputActionKey::kMouseScrollUp),

		new InputBinding(GamepadButtons::kLThumbUp, InputActionKey::kMouseUp),
		new InputBinding(GamepadButtons::kLThumbLeft, InputActionKey::kMouseLeft),
		new InputBinding(GamepadButtons::kLThumbRight, InputActionKey::kMouseRight),
		new InputBinding(GamepadButtons::kLThumbDown, InputActionKey::kMouseDown),
		new InputBinding(GamepadButtons::kRThumbUp, InputActionKey::kMouseUp),
		new InputBinding(GamepadButtons::kRThumbLeft, InputActionKey::kMouseLeft),
		new InputBinding(GamepadButtons::kRThumbRight, InputActionKey::kMouseRight),
		new InputBinding(GamepadButtons::kRThumbDown, InputActionKey::kMouseDown),

		new InputBinding(GamepadButtons::kLUp, InputActionKey::kArrowUp),
		new InputBinding(GamepadButtons::kLLeft, InputActionKey::kArrowLeft),
		new InputBinding(GamepadButtons::kLRight, InputActionKey::kArrowRight),
		new InputBinding(GamepadButtons::kLDown, InputActionKey::kArrowDown),
	};

	return std::move(defaultPreset);
}

const FilePath InputPreset::GetFilePath_Internal()
{
	std::wstring name(m_name.begin(), m_name.end());
	return FilePath(InputPresetManager::GetDirectory(), name);
}
