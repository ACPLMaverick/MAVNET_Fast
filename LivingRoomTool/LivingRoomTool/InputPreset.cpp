#include "InputPreset.h"
#include "InputPresetManager.h"

InputPreset::InputPreset(const char * name)
	: Serializable(name)
{
}

InputPreset::InputPreset(InputPreset && a_move)
	: Serializable(a_move.m_name.c_str())
{
	m_bindings = a_move.m_bindings;
	a_move.m_bindings.Get().clear();
}

InputPreset::InputPreset(InputPreset && a_move, const char * a_newName)
	: Serializable(a_newName)
{
	m_bindings = a_move.m_bindings;
	a_move.m_bindings.Get().clear();
}

InputPreset::~InputPreset()
{
	for (InputBinding* binding : m_bindings.Get())
	{
		delete binding;
	}
	m_bindings.Get().clear();
}

InputPreset & InputPreset::operator=(InputPreset && a_move)
{
	m_name = a_move.m_name;
	m_bindings = a_move.m_bindings;
	a_move.m_bindings.Get().clear();
	return *this;
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

bool InputPreset::IsDefault() const
{
	return GetName() == k_defaultName;
}

InputPreset InputPreset::CreateDefault()
{
	InputPreset defaultPreset(k_defaultName);

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

InputPreset InputPreset::DeepCopy(const InputPreset & a_copy, const char* a_newName)
{
	InputPreset preset(a_newName);

	for (InputBinding* binding : a_copy.Get_bindings())
	{
		InputBinding* newBinding = new InputBinding(binding->Get_sources(), binding->Get_destinations());
		preset.Get_bindings().push_back(newBinding);
	}
	return std::move(preset);
}

const FilePath InputPreset::GetFilePath_Internal()
{
	std::wstring name(m_name.begin(), m_name.end());
	return FilePath(InputPresetManager::GetDirectory(), name);
}

const char* InputPreset::k_defaultName("default");
