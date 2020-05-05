#pragma once

#include "Common.h"

#include "AdvancedBindingEditor.h"

class InputPreset;
class InputPresetManager;
class LivingRoomTool;
class InputBinding;
class QComboBox;
class QListWidget;
class QPushButton;
enum class GamepadButtons : uint32_t;

class PresetEditor : public QObject
{
	Q_OBJECT

public:

	struct Elements
	{
		static const size_t k_comboBoxNum = 24;

		union
		{
			struct
			{
				QComboBox* Cb_RT;
				QComboBox* Cb_RB;
				QComboBox* Cb_LT;
				QComboBox* Cb_LB;
				QComboBox* Cb_View;
				QComboBox* Cb_Menu;
				QComboBox* Cb_RUp;
				QComboBox* Cb_RLeft;
				QComboBox* Cb_RDown;
				QComboBox* Cb_RRight;
				QComboBox* Cb_LDown;
				QComboBox* Cb_LLeft;
				QComboBox* Cb_LRight;
				QComboBox* Cb_LUp;
				QComboBox* Cb_RThumbPress;
				QComboBox* Cb_LThumbPress;
				QComboBox* Cb_LThumbDown;
				QComboBox* Cb_LThumbUp;
				QComboBox* Cb_LThumbLeft;
				QComboBox* Cb_LThumbRight;
				QComboBox* Cb_RThumbUp;
				QComboBox* Cb_RThumbDown;
				QComboBox* Cb_RThumbLeft;
				QComboBox* Cb_RThumbRight;
			};
			QComboBox* ComboBoxes[k_comboBoxNum];
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

	inline void InitializeComboboxes();
	inline void InitializeLookups();
	inline void InitializeConnections();
	inline void UpdateElements();

	// Will return false if corresponding combobox has already an assigned value.
	bool UpdateCombobox(InputBinding* binding);
	void UpdateAdvancedList(InputBinding* binding);

	std::string GetInputBindingAsString(const InputBinding* binding);
	class QComboBox* GetComboBox(GamepadButtons button);
	GamepadButtons GetGamepadButtons(class QComboBox* comboBox);
	InputBinding* GetSimpleInputBinding(class QComboBox* comboBox);
	InputBinding* TryGetSimpleInputBinding(class QComboBox* comboBox);
	InputBinding* GetAdvancedInputBinding(size_t index);

	void OnComboBoxIndexChanged(QComboBox* comboBox, int newIdx);
	void OnAdvancedAddClicked();
	void OnAdvancedDuplicateClicked();
	void OnAdvancedEditClicked();
	void OnAdvancedRemoveClicked();

	void AddInputBindingToPreset(InputBinding* binding);
	void RemoveInputBindingFromPreset(InputBinding* binding);

	void RemoveSimpleBinding(QComboBox* comboBox, InputBinding* binding);

	void AddAdvancedBinding();
	void DuplicateAdvancedBinding(size_t bindingIndex);
	void EditAdvancedBinding(size_t bindingIndex);
	void RemoveAdvancedBinding(size_t bindingIndex);

	static const size_t k_invalidIndex = static_cast<size_t>(-1);

	Elements m_elements;
	
	std::map<GamepadButtons, QComboBox*> m_gamepadButtonsToComboBox;
	std::map<QComboBox*, GamepadButtons> m_comboBoxToGamepadButtons;

	std::map<QComboBox*, InputBinding*> m_simpleBindingsLookup;
	std::vector<InputBinding*> m_advancedBindingsLookup;

	InputPresetManager* m_presetManager{ nullptr };
	LivingRoomTool* m_mainWidget{ nullptr };
	size_t m_assignedPresetIndex{ k_invalidIndex };

	bool m_bDisableConnections{ false };
};

