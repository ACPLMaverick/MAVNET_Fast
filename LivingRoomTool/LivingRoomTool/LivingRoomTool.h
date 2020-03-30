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
	void OnSaveTweakClicked();
	void OnRestoreTweakClicked();
	void OnDefaultsTweakClicked();

	void OnDeviceSelectionChanged();
	void OnPresetSelectionChanged();

	void UpdatePanelsForSelectedDevice(size_t selectedDevice);
	void UpdatePanelsForSelectedDevice_Presets(size_t selectedDevice);
	void UpdatePanelsForSelectedDevice_Tweaks(size_t selectedDevice);
	void UpdateEditorForSelectedPreset(size_t selectedPreset);
	void ClearEditor();

	void SetQLayoutElementsFrozen(QLayout* layout, bool frozen);
	size_t GetQListSelectedIndex(QListWidget* list);
	size_t GetGamepadSelectedIndex();
	size_t GetPresetSelectedIndex();

	Ui::LivingRoomToolClass ui;
	InputProcessor m_inputProcessor;
	PresetEditor m_presetEditor;

	static const size_t k_invalidIndex = static_cast<size_t>(-1);
};
