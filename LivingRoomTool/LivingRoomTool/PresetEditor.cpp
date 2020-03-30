#include "PresetEditor.h"

#include "InputPresetManager.h"
#include "InputAction.h"

#include <QtWidgets/QComboBox>
#include <QtWidgets/QListWidget.h>
#include <QtWidgets/QPushButton.h>

#define LRT_ValidateAssignedIndex() LRT_Assert(m_assignedPresetIndex != k_invalidIndex)

PresetEditor::PresetEditor()
	: QObject(Q_NULLPTR)
{
}

PresetEditor::~PresetEditor()
{

}

void PresetEditor::Init(const Elements & a_elements, InputPresetManager* a_manager)
{
	m_elements = a_elements;
	m_presetManager = a_manager;

	InitializeComboboxes();
	InitializeLookups();
	InitializeConnections();
}

void PresetEditor::Cleanup()
{
	m_presetManager = nullptr;
	m_assignedPresetIndex = k_invalidIndex;
	m_gamepadButtonsToComboBox.clear();
	m_comboBoxToGamepadButtons.clear();
	m_simpleBindingsLookup.clear();
	m_advancedBindingsLookup.clear();
	m_bDisableConnections = false;
	memset(&m_elements, 0, sizeof(Elements));
}

void PresetEditor::AssignPreset(size_t a_assignedPresetIndex)
{
	m_assignedPresetIndex = a_assignedPresetIndex;
	LRT_ValidateAssignedIndex();
	UpdateElements();
}

void PresetEditor::InvalidatePreset()
{
	m_assignedPresetIndex = k_invalidIndex;
}

inline InputPreset& PresetEditor::GetPreset()
{
	LRT_ValidateAssignedIndex();
	return m_presetManager->GetPreset(m_assignedPresetIndex);
}

inline void PresetEditor::InitializeComboboxes()
{
	for (size_t i = 0; i < Elements::k_comboBoxNum; ++i)
	{
		QComboBox* cb = m_elements.ComboBoxes[i];

		for (size_t j = 0; j < static_cast<size_t>(InputActionKey::ENUM_SIZE); ++j)
		{
			const std::string_view& str = magic_enum::enum_name(static_cast<InputActionKey>(j));
			LRT_Assert(str.size() > 0);
			if (str.size() == 0)
			{
				continue;
			}

			cb->addItem(QString::fromUtf8(str.data(), str.size()));
		}
	}
}

inline void PresetEditor::InitializeLookups()
{
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kLT, m_elements.Cb_LT);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kLB, m_elements.Cb_LB);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kLThumb, m_elements.Cb_LThumbPress);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kLLeft, m_elements.Cb_LLeft);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kLUp, m_elements.Cb_LUp);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kLRight, m_elements.Cb_LRight);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kLDown, m_elements.Cb_LDown);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kView, m_elements.Cb_View);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kMenu, m_elements.Cb_Menu);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kRT, m_elements.Cb_RT);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kRB, m_elements.Cb_RB);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kRThumb, m_elements.Cb_RThumbPress);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kRLeft, m_elements.Cb_RLeft);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kRUp, m_elements.Cb_RUp);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kRRight, m_elements.Cb_RRight);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kRDown, m_elements.Cb_RDown);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kLThumbLeft, m_elements.Cb_LThumbLeft);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kLThumbUp, m_elements.Cb_LThumbUp);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kLThumbRight, m_elements.Cb_LThumbRight);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kLThumbDown, m_elements.Cb_LThumbDown);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kRThumbLeft, m_elements.Cb_RThumbLeft);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kRThumbUp, m_elements.Cb_RThumbUp);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kRThumbRight, m_elements.Cb_RThumbRight);
	m_gamepadButtonsToComboBox.emplace(GamepadButtons::kRThumbDown, m_elements.Cb_RThumbDown);

	for (std::pair<const GamepadButtons, QComboBox*>& pair : m_gamepadButtonsToComboBox)
	{
		m_comboBoxToGamepadButtons.emplace(pair.second, pair.first);
	}
}

inline void PresetEditor::InitializeConnections()
{
	for (size_t i = 0; i < Elements::k_comboBoxNum; ++i)
	{
		void (QComboBox::*indexChangedSignal)(int) = &QComboBox::currentIndexChanged;
		connect(m_elements.ComboBoxes[i], indexChangedSignal, this, [=](int newIdx) 
		{
			OnComboBoxIndexChanged(m_elements.ComboBoxes[i], newIdx);
		});
	}

	connect(m_elements.Btn_AdvancedAdd, &QAbstractButton::clicked, this, &PresetEditor::OnAdvancedAddClicked);
	connect(m_elements.Btn_AdvancedDuplicate, &QAbstractButton::clicked, this, &PresetEditor::OnAdvancedDuplicateClicked);
	connect(m_elements.Btn_AdvancedEdit, &QAbstractButton::clicked, this, &PresetEditor::OnAdvancedEditClicked);
	connect(m_elements.Btn_AdvancedRemove, &QAbstractButton::clicked, this, &PresetEditor::OnAdvancedRemoveClicked);
}

inline void PresetEditor::UpdateElements()
{
	if (m_assignedPresetIndex == k_invalidIndex)
	{
		return;
	}

	m_simpleBindingsLookup.clear();
	m_advancedBindingsLookup.clear();

	const InputPreset& preset = GetPreset();
	const std::vector<InputBinding*>& bindings = preset.Get_bindings();

	m_bDisableConnections = true;

	for (InputBinding* binding : bindings)
	{
		const size_t sourcesSize = binding->Get_sources().size();
		const size_t destinationsSize = binding->Get_destinations().size();

		if (sourcesSize == 0 || destinationsSize == 0)
		{
			LRT_Fail();
			continue;
		}
		else if (sourcesSize == 1 && destinationsSize == 1)
		{
			UpdateCombobox(binding);
		}
		else
		{
			UpdateAdvancedList(binding);
		}
	}

	m_bDisableConnections = false;
}

void PresetEditor::UpdateCombobox(InputBinding * a_binding)
{
	const GamepadButtons source = a_binding->Get_sources()[0];
	const InputActionKey destination = a_binding->Get_destinations()[0];
	QComboBox* comboBox = GetComboBox(source);
	comboBox->setCurrentIndex(static_cast<int>(destination));
	m_simpleBindingsLookup.emplace(comboBox, a_binding);
}

void PresetEditor::UpdateAdvancedList(InputBinding * a_binding)
{
	m_elements.List_AdvancedBindings->addItem(QString::fromStdString(GetInputBindingAsString(a_binding)));
	m_advancedBindingsLookup.push_back(a_binding);
}

inline std::string PresetEditor::GetInputBindingAsString(const InputBinding * a_binding)
{
	std::string text = "[";
	const InputBinding::Sources& sources = a_binding->Get_sources();
	const InputBinding::Destinations& destinations = a_binding->Get_destinations();

	for (GamepadButtons button : sources)
	{
		const std::string_view& strButton = GamepadButtonsConvert::ToString(button);
		LRT_Assert(strButton.size() > 0);
		
		text += strButton;
		if (button == sources[sources.size() - 1])
		{
			text += "]";
		}
		else
		{
			text += ", ";
		}
	}

	text += " -> [";

	for (InputActionKey key : destinations)
	{
		const std::string_view& strKey = magic_enum::enum_name(key);
		LRT_Assert(strKey.size() > 0);

		text += strKey;
		if (key == destinations[destinations.size() - 1])
		{
			text += "]";
		}
		else
		{
			text += ", ";
		}
	}

	return text;
}

QComboBox* PresetEditor::GetComboBox(GamepadButtons a_button)
{
	auto it  = m_gamepadButtonsToComboBox.find(a_button);
	LRT_Assert(it != m_gamepadButtonsToComboBox.end());
	return it->second;
}

GamepadButtons PresetEditor::GetGamepadButtons(QComboBox* a_comboBox)
{
	auto it = m_comboBoxToGamepadButtons.find(a_comboBox);
	LRT_Assert(it != m_comboBoxToGamepadButtons.end());
	return it->second;
}

InputBinding* PresetEditor::GetSimpleInputBinding(QComboBox* a_comboBox)
{
	auto it = m_simpleBindingsLookup.find(a_comboBox);
	LRT_Assert(it != m_simpleBindingsLookup.end());
	return it->second;
}

InputBinding* PresetEditor::TryGetSimpleInputBinding(class QComboBox* a_comboBox)
{
	auto it = m_simpleBindingsLookup.find(a_comboBox);
	if (it != m_simpleBindingsLookup.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

InputBinding* PresetEditor::GetAdvancedInputBinding(size_t a_index)
{
	LRT_Assert(a_index < m_advancedBindingsLookup.size());
	return m_advancedBindingsLookup[a_index];
}

void PresetEditor::OnComboBoxIndexChanged(QComboBox * a_comboBox, int a_newIdx)
{
	if (m_bDisableConnections)
	{
		return;
	}

	LRT_Assert(a_newIdx < static_cast<int>(InputActionKey::ENUM_SIZE));

	InputBinding* binding = TryGetSimpleInputBinding(a_comboBox);
	if (binding == nullptr)
	{
		// Create a new simple InputBinding.
		InputBinding* binding = new InputBinding();
		binding->Get_sources().push_back(GetGamepadButtons(a_comboBox));
		binding->Get_destinations().push_back(static_cast<InputActionKey>(a_newIdx));

		GetPreset().Get_bindings().push_back(binding);
		m_simpleBindingsLookup.emplace(a_comboBox, binding);
	}
	else
	{
		binding->Get_destinations()[0] = static_cast<InputActionKey>(a_newIdx);
	}
}

void PresetEditor::OnAdvancedAddClicked()
{
	if (m_bDisableConnections)
	{
		return;
	}
	// TODO: Open Advanced Editor.
}

void PresetEditor::OnAdvancedDuplicateClicked()
{
	if (m_bDisableConnections)
	{
		return;
	}
	// TODO: Duplicate an Advanced Binding.
}

void PresetEditor::OnAdvancedEditClicked()
{
	if (m_bDisableConnections)
	{
		return;
	}
	// TODO: Edit an Advanced Binding.
}

void PresetEditor::OnAdvancedRemoveClicked()
{
	if (m_bDisableConnections)
	{
		return;
	}
	// TODO: Remove an Advanced Binding. Display a confirmation prompt.
}
