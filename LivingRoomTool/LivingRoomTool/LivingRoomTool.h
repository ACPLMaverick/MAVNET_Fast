#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_LivingRoomTool.h"

#include "InputProcessor.h"
#include "PresetEditor.h"

class LivingRoomTool : public QMainWindow
{
	Q_OBJECT

public:
	LivingRoomTool(QWidget *parent = Q_NULLPTR);
	~LivingRoomTool();

private:

	void InitTweakNames();
	void InitConnections();
	void InitPresetEditor();

	void CleanupPresetEditor();

	void EnableDevicePanels();
	void DisableDevicePanels();
	void EnablePresetEditor();
	void DisablePresetEditor();

	void OnRefreshDevicesClicked();
	void OnIdentifyDeviceClicked();
	void OnDuplicatePresetClicked();
	void OnRemovePresetClicked();
	void OnRenamePresetClicked();
	void OnSavePresetClicked();
	void OnRestorePresetClicked();
	void OnSaveTweakClicked();
	void OnRestoreTweakClicked();
	void OnDefaultsTweakClicked();

	void OnDeviceSelectionChanged();
	void OnPresetSelectionChanged();

	void UpdatePanelsForSelectedDevice(size_t selectedDevice);
	void UpdatePanels_Presets();
	void UpdatePanelsForSelectedDevice_Tweaks(size_t selectedDevice);
	void UpdateEditorForSelectedPreset(size_t selectedPreset);
	void UpdatePresetButtonAvailabilityForSelectedPreset(size_t selectedPreset);
	void ClearEditor();

	bool AskForPresetNameWithExistenceCheck(
		size_t selectedPreset,
		std::string& outName,
		const wchar_t* askTitle,
		const wchar_t* askErrorMessage,
		const char* nameSuffix = nullptr);

	void SetQLayoutElementsFrozen(QLayout* layout, bool frozen);
	size_t GetQListSelectedIndex(QListWidget* list);
	size_t GetGamepadSelectedIndex();
	size_t GetPresetSelectedIndex();

	Ui::LivingRoomToolClass ui;
	InputProcessor m_inputProcessor;
	PresetEditor m_presetEditor;
};
