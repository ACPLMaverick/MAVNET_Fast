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
	void BtnTest_ToggleTest();

	Ui::LivingRoomToolClass ui;
	InputProcessor m_inputProcessor;
};
