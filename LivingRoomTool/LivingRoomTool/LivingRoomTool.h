#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_LivingRoomTool.h"

#include "InputProcessor.h"

class LivingRoomTool : public QMainWindow
{
	Q_OBJECT

public:
	LivingRoomTool(QWidget *parent = Q_NULLPTR);

private:

	void InitTweakNames();
	void InitConnections();

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

	void SetQLayoutElementsFrozen(QLayout* layout, bool frozen);
	size_t GetGamepadSelectedIndex();

	Ui::LivingRoomToolClass ui;
	InputProcessor m_inputProcessor;

	static const size_t k_invalidIndex = static_cast<size_t>(-1);
};
