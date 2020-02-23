#include "LivingRoomTool.h"

LivingRoomTool::LivingRoomTool(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.BtnTest, &QPushButton::released, this, &LivingRoomTool::BtnTest_ToggleTest);
}

void LivingRoomTool::BtnTest_ToggleTest()
{
	m_inputProcessor.Toggle();
}
