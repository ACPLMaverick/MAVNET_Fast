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

	void InitVisibilities();
	void FillDeviceList();
	void OnDeviceSelectedShowAndFillPresetList();
	void OnDeviceDeselectedHidePresetList();
	void OnPresetSelectedShowAndFillGamepad();
	void OnPresetDeselectedHideGamepad();

	void SetQLayoutElementsFrozen(QLayout* layout, bool frozen);

	Ui::LivingRoomToolClass ui;
	InputProcessor m_inputProcessor;
};
