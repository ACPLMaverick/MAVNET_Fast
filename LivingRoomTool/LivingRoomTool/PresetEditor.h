#pragma once

#include "Common.h"

#include "AdvancedBindingEditor.h"

class InputPreset;
class InputPresetManager;
class LivingRoomTool;
class InputBinding;
class BindingSelectorWidget;
class QListWidget;
class QPushButton;
enum class GamepadButtons : uint32_t;

class PresetEditor : public QObject
{
	Q_OBJECT

public:

	struct Elements
	{
		static const size_t k_bindingSelectorNum = 24;

		union
		{
			struct
			{
				BindingSelectorWidget* Bs_RT;
				BindingSelectorWidget* Bs_RB;
				BindingSelectorWidget* Bs_LT;
				BindingSelectorWidget* Bs_LB;
				BindingSelectorWidget* Bs_View;
				BindingSelectorWidget* Bs_Menu;
				BindingSelectorWidget* Bs_RUp;
				BindingSelectorWidget* Bs_RLeft;
				BindingSelectorWidget* Bs_RDown;
				BindingSelectorWidget* Bs_RRight;
				BindingSelectorWidget* Bs_LDown;
				BindingSelectorWidget* Bs_LLeft;
				BindingSelectorWidget* Bs_LRight;
				BindingSelectorWidget* Bs_LUp;
				BindingSelectorWidget* Bs_RThumbPress;
				BindingSelectorWidget* Bs_LThumbPress;
				BindingSelectorWidget* Bs_LThumbDown;
				BindingSelectorWidget* Bs_LThumbUp;
				BindingSelectorWidget* Bs_LThumbLeft;
				BindingSelectorWidget* Bs_LThumbRight;
				BindingSelectorWidget* Bs_RThumbUp;
				BindingSelectorWidget* Bs_RThumbDown;
				BindingSelectorWidget* Bs_RThumbLeft;
				BindingSelectorWidget* Bs_RThumbRight;
			};
			BindingSelectorWidget* BindingSelectors[k_bindingSelectorNum];
		};
		QListWidget* List_AdvancedBindings;
		QPushButton* Btn_AdvancedAdd;
		QPushButton* Btn_AdvancedDuplicate;
		QPushButton* Btn_AdvancedEdit;
		QPushButton* Btn_AdvancedRemove;
	};

	PresetEditor();
	~PresetEditor();

	void Init(const Elements& elements, InputPresetManager* manager, LivingRoomTool* mainWidget);
	void Cleanup();

	void AssignPreset(size_t assignedPresetIndex);
	void InvalidatePreset();

private:

	inline InputPreset& GetPreset();

	inline void InitializeBindingSelectors();
	inline void InitializeLookups();
	inline void InitializeConnections();
	inline void UpdateElements();

	// Will return false if corresponding BindingSelector has already an assigned value.
	bool UpdateBindingSelector(InputBinding* binding);
	void UpdateAdvancedList(InputBinding* binding);

	std::string GetInputBindingAsString(const InputBinding* binding);
	class BindingSelectorWidget* GetBindingSelector(GamepadButtons button);
	GamepadButtons GetGamepadButtons(class BindingSelectorWidget* bindingSelector);
	InputBinding* GetSimpleInputBinding(class BindingSelectorWidget* bindingSelector);
	InputBinding* TryGetSimpleInputBinding(class BindingSelectorWidget* bindingSelector);
	InputBinding* GetAdvancedInputBinding(size_t index);

	void OnBindingSelectorIndexChanged(BindingSelectorWidget* bindingSelector, int newIdx);
	void OnAdvancedAddClicked();
	void OnAdvancedDuplicateClicked();
	void OnAdvancedEditClicked();
	void OnAdvancedRemoveClicked();

	void AddInputBindingToPreset(InputBinding* binding);
	void RemoveInputBindingFromPreset(InputBinding* binding);

	void RemoveSimpleBinding(BindingSelectorWidget* bindingSelector, InputBinding* binding);

	void AddAdvancedBinding();
	void DuplicateAdvancedBinding(size_t bindingIndex);
	void EditAdvancedBinding(size_t bindingIndex);
	void RemoveAdvancedBinding(size_t bindingIndex);

	static const size_t k_invalidIndex = static_cast<size_t>(-1);

	Elements m_elements;
	
	std::map<GamepadButtons, BindingSelectorWidget*> m_gamepadButtonsToBindingSelector;
	std::map<BindingSelectorWidget*, GamepadButtons> m_bindingSelectorToGamepadButtons;

	std::map<BindingSelectorWidget*, InputBinding*> m_simpleBindingsLookup;
	std::vector<InputBinding*> m_advancedBindingsLookup;

	InputPresetManager* m_presetManager{ nullptr };
	LivingRoomTool* m_mainWidget{ nullptr };
	size_t m_assignedPresetIndex{ k_invalidIndex };

	bool m_bDisableConnections{ false };
};

