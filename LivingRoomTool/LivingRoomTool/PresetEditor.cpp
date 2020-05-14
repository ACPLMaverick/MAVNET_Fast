#include "PresetEditor.h"

#include "LivingRoomTool.h"
#include "InputPresetManager.h"
#include "InputAction.h"
#include "SimpleConfirmationDialog.h"
#include "BindingSelectorWidget.h"

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

void PresetEditor::Init(const Elements & a_elements, InputPresetManager* a_manager, LivingRoomTool* a_mainWidget)
{
	m_elements = a_elements;
	m_presetManager = a_manager;
	m_mainWidget = a_mainWidget;

	InitializeBindingSelectors();
	InitializeLookups();
	InitializeConnections();
}

void PresetEditor::Cleanup()
{
	m_presetManager = nullptr;
	m_assignedPresetIndex = k_invalidIndex;
	m_gamepadButtonsToBindingSelector.clear();
	m_bindingSelectorToGamepadButtons.clear();
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
	m_elements.List_AdvancedBindings->clear();

	m_bDisableConnections = true;
	for (BindingSelectorWidget* bs : m_elements.BindingSelectors)
	{
		bs->SetIndex(0);
	}
	m_bDisableConnections = false;
}

inline InputPreset& PresetEditor::GetPreset()
{
	LRT_ValidateAssignedIndex();
	return m_presetManager->GetPreset(m_assignedPresetIndex);
}

inline void PresetEditor::InitializeBindingSelectors()
{
	for (size_t i = 0; i < Elements::k_bindingSelectorNum; ++i)
	{
		BindingSelectorWidget* bs = m_elements.BindingSelectors[i];
		bs->Fill<InputActionKey>();
	}
}

inline void PresetEditor::InitializeLookups()
{
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kLT, m_elements.Bs_LT);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kLB, m_elements.Bs_LB);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kLThumb, m_elements.Bs_LThumbPress);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kLLeft, m_elements.Bs_LLeft);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kLUp, m_elements.Bs_LUp);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kLRight, m_elements.Bs_LRight);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kLDown, m_elements.Bs_LDown);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kView, m_elements.Bs_View);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kMenu, m_elements.Bs_Menu);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kRT, m_elements.Bs_RT);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kRB, m_elements.Bs_RB);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kRThumb, m_elements.Bs_RThumbPress);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kRLeft, m_elements.Bs_RLeft);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kRUp, m_elements.Bs_RUp);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kRRight, m_elements.Bs_RRight);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kRDown, m_elements.Bs_RDown);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kLThumbLeft, m_elements.Bs_LThumbLeft);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kLThumbUp, m_elements.Bs_LThumbUp);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kLThumbRight, m_elements.Bs_LThumbRight);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kLThumbDown, m_elements.Bs_LThumbDown);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kRThumbLeft, m_elements.Bs_RThumbLeft);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kRThumbUp, m_elements.Bs_RThumbUp);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kRThumbRight, m_elements.Bs_RThumbRight);
	m_gamepadButtonsToBindingSelector.emplace(GamepadButtons::kRThumbDown, m_elements.Bs_RThumbDown);

	for (std::pair<const GamepadButtons, BindingSelectorWidget*>& pair : m_gamepadButtonsToBindingSelector)
	{
		m_bindingSelectorToGamepadButtons.emplace(pair.second, pair.first);
	}
}

inline void PresetEditor::InitializeConnections()
{
	for (size_t i = 0; i < Elements::k_bindingSelectorNum; ++i)
	{
		connect(m_elements.BindingSelectors[i], &BindingSelectorWidget::indexChanged, this, [=](int newIdx) 
		{
			OnBindingSelectorIndexChanged(m_elements.BindingSelectors[i], newIdx);
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

	// First set all UIs to default state.
	for (BindingSelectorWidget* cb : m_elements.BindingSelectors)
	{
		cb->SetIndex(0);
	}
	m_elements.List_AdvancedBindings->clear();

	// Now update UIs with new state.
	for (InputBinding* binding : bindings)
	{
		const size_t sourcesSize = binding->Get_sources().size();
		const size_t destinationsSize = binding->Get_destinations().size();

		if (sourcesSize == 0 || destinationsSize == 0)
		{
			LRT_Fail();	// Empty binding should not be present in the array!
			continue;
		}
		else if (sourcesSize == 1 && destinationsSize == 1)
		{
			if (UpdateBindingSelector(binding) == false)
			{
				UpdateAdvancedList(binding);
			}
		}
		else
		{
			UpdateAdvancedList(binding);
		}
	}

	m_bDisableConnections = false;
}

bool PresetEditor::UpdateBindingSelector(InputBinding * a_binding)
{
	const GamepadButtons source = a_binding->Get_sources()[0];
	const InputActionKey destination = a_binding->Get_destinations()[0];
	BindingSelectorWidget* bindingSelector = GetBindingSelector(source);
	if (bindingSelector->GetIndex() == 0)
	{
		bindingSelector->SetIndex(static_cast<int>(destination));
		m_simpleBindingsLookup.emplace(bindingSelector, a_binding);
		return true;
	}
	else
	{
		return false;
	}
}

void PresetEditor::UpdateAdvancedList(InputBinding* a_binding)
{
	m_elements.List_AdvancedBindings->addItem(QString::fromStdString(GetInputBindingAsString(a_binding)));
	m_advancedBindingsLookup.push_back(a_binding);
}

std::string PresetEditor::GetInputBindingAsString(const InputBinding * a_binding)
{
	std::string text = "[";
	const InputBinding::Sources& sources = a_binding->Get_sources();
	const InputBinding::Destinations& destinations = a_binding->Get_destinations();

	for (GamepadButtons button : sources)
	{
		const std::string_view& strButton = GamepadButtonsHelper::ToString(button);
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

BindingSelectorWidget* PresetEditor::GetBindingSelector(GamepadButtons a_button)
{
	auto it  = m_gamepadButtonsToBindingSelector.find(a_button);
	LRT_Assert(it != m_gamepadButtonsToBindingSelector.end());
	return it->second;
}

GamepadButtons PresetEditor::GetGamepadButtons(BindingSelectorWidget* a_bindingSelector)
{
	auto it = m_bindingSelectorToGamepadButtons.find(a_bindingSelector);
	LRT_Assert(it != m_bindingSelectorToGamepadButtons.end());
	return it->second;
}

InputBinding* PresetEditor::GetSimpleInputBinding(BindingSelectorWidget* a_bindingSelector)
{
	auto it = m_simpleBindingsLookup.find(a_bindingSelector);
	LRT_Assert(it != m_simpleBindingsLookup.end());
	return it->second;
}

InputBinding* PresetEditor::TryGetSimpleInputBinding(BindingSelectorWidget* a_bindingSelector)
{
	auto it = m_simpleBindingsLookup.find(a_bindingSelector);
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

void PresetEditor::OnBindingSelectorIndexChanged(BindingSelectorWidget* a_bindingSelector, int a_newIdx)
{
	if (m_bDisableConnections)
	{
		return;
	}

	LRT_Assert(a_newIdx < static_cast<int>(InputActionKey::ENUM_SIZE));

	InputBinding* binding = TryGetSimpleInputBinding(a_bindingSelector);

	if (static_cast<InputActionKey>(a_newIdx) == InputActionKey::kNone)
	{
		// Remove that simple input binding.
		LRT_Assert(binding != nullptr);
		RemoveSimpleBinding(a_bindingSelector, binding);
	}
	else
	{
		if (binding == nullptr)
		{
			// Create a new simple InputBinding.
			InputBinding* binding = new InputBinding(GetGamepadButtons(a_bindingSelector), static_cast<InputActionKey>(a_newIdx));

			GetPreset().Get_bindings().push_back(binding);
			m_simpleBindingsLookup.emplace(a_bindingSelector, binding);
		}
		else
		{
			binding->SetDestination(static_cast<InputActionKey>(a_newIdx));
		}
	}
}

void PresetEditor::OnAdvancedAddClicked()
{
	if (m_bDisableConnections)
	{
		return;
	}
	AddAdvancedBinding();
}

void PresetEditor::OnAdvancedDuplicateClicked()
{
	if (m_bDisableConnections)
	{
		return;
	}
	DuplicateAdvancedBinding(LRT_QTHelper::GetQListSelectedIndex(m_elements.List_AdvancedBindings));
}

void PresetEditor::OnAdvancedEditClicked()
{
	if (m_bDisableConnections)
	{
		return;
	}
	EditAdvancedBinding(LRT_QTHelper::GetQListSelectedIndex(m_elements.List_AdvancedBindings));
}

void PresetEditor::OnAdvancedRemoveClicked()
{
	if (m_bDisableConnections)
	{
		return;
	}
	RemoveAdvancedBinding(LRT_QTHelper::GetQListSelectedIndex(m_elements.List_AdvancedBindings));
}

void PresetEditor::AddInputBindingToPreset(InputBinding* a_binding)
{
	GetPreset().Get_bindings().push_back(a_binding);
}

void PresetEditor::RemoveInputBindingFromPreset(InputBinding* a_binding)
{
	InputPreset& preset = GetPreset();
	std::vector<InputBinding*>& bindings = preset.Get_bindings();
	bindings.erase(std::find(bindings.begin(), bindings.end(), a_binding));
}

void PresetEditor::RemoveSimpleBinding(BindingSelectorWidget* a_bindingSelector, InputBinding* a_binding)
{
	m_simpleBindingsLookup.erase(a_bindingSelector);
	RemoveInputBindingFromPreset(a_binding);
}

void PresetEditor::AddAdvancedBinding()
{
	InputBinding* newBinding = new InputBinding();

	AdvancedBindingEditor editor;
	if (editor.Open(m_mainWidget, *newBinding))
	{
		// Add to preset.
		AddInputBindingToPreset(newBinding);

		// Update lookup and list.
		UpdateAdvancedList(newBinding);
	}
	else
	{
		delete newBinding;
	}
}

void PresetEditor::DuplicateAdvancedBinding(size_t a_bindingIndex)
{
	if (a_bindingIndex >= m_advancedBindingsLookup.size())
	{
		return;
	}

	InputBinding* newBinding = new InputBinding(*m_advancedBindingsLookup[a_bindingIndex]);

	AdvancedBindingEditor editor;
	if (editor.Open(m_mainWidget, *newBinding))
	{
		// Add to preset.
		AddInputBindingToPreset(newBinding);

		// Update lookup and list.
		UpdateAdvancedList(newBinding);
	}
	else
	{
		delete newBinding;
	}
}

void PresetEditor::EditAdvancedBinding(size_t a_bindingIndex)
{
	if (a_bindingIndex >= m_advancedBindingsLookup.size())
	{
		return;
	}

	AdvancedBindingEditor editor;
	if (editor.Open(m_mainWidget, *m_advancedBindingsLookup[a_bindingIndex]))
	{
		m_elements.List_AdvancedBindings->item(a_bindingIndex)->setText(QString::fromStdString(GetInputBindingAsString(m_advancedBindingsLookup[a_bindingIndex])));
	}
}

void PresetEditor::RemoveAdvancedBinding(size_t a_bindingIndex)
{
	if (a_bindingIndex >= m_advancedBindingsLookup.size())
	{
		return;
	}

	if (SimpleConfirmationDialog::Open("Do you really want to remove this binding?") == false)
	{
		return;
	}

	// Remove from preset.
	InputBinding* binding = m_advancedBindingsLookup[a_bindingIndex];
	RemoveInputBindingFromPreset(binding);

	// Remove from lookup and list.
	m_advancedBindingsLookup.erase(m_advancedBindingsLookup.begin() + a_bindingIndex);
	QListWidgetItem* item = m_elements.List_AdvancedBindings->takeItem(static_cast<int>(a_bindingIndex));
	delete item;
}