#include "LivingRoomTool.h"

LivingRoomTool::LivingRoomTool(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	InitVisibilities();
	m_inputProcessor.Init();
	FillDeviceList();
}

void LivingRoomTool::InitVisibilities()
{
	SetQLayoutElementsFrozen(ui.VertLayout_Presets, true);
	SetQLayoutElementsFrozen(ui.VertLayout_EditPreset, true);
}

void LivingRoomTool::FillDeviceList()
{
}

void LivingRoomTool::OnDeviceSelectedShowAndFillPresetList()
{
}

void LivingRoomTool::OnDeviceDeselectedHidePresetList()
{
}

void LivingRoomTool::OnPresetSelectedShowAndFillGamepad()
{
}

void LivingRoomTool::OnPresetDeselectedHideGamepad()
{
}

void LivingRoomTool::SetQLayoutElementsFrozen(QLayout * a_layout, bool a_frozen)
{
	std::stack<QLayout*> layouts;
	layouts.push(a_layout);

	while (layouts.empty() == false)
	{
		QLayout* layout = layouts.top();
		layouts.pop();

		const int count = layout->count();
		for (int i = 0; i < count; ++i)
		{
			QWidget* widget = layout->itemAt(i)->widget();
			if (widget != nullptr)
			{
				widget->setEnabled(!a_frozen);
			}
			else
			{
				QLayout* itemLayout = layout->itemAt(i)->layout();
				if (itemLayout != nullptr)
				{
					layouts.push(itemLayout);
				}
			}
		}
	}
}