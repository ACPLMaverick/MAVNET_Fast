#include "LivingRoomTool.h"

LivingRoomTool::LivingRoomTool(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	InitVisibilities();
	InitConnections();
	InitTweakNames();

	m_inputProcessor.Init();

	OnRefreshDevicesClicked();
}

void LivingRoomTool::InitVisibilities()
{
	SetQLayoutElementsFrozen(ui.VertLayout_Presets, true);
	SetQLayoutElementsFrozen(ui.VertLayout_EditPreset, true);
	SetQLayoutElementsFrozen(ui.VertLayout_Tweaks, true);
}

void LivingRoomTool::InitTweakNames()
{
	ui.Tweak_Deadzone_LX->SetName(L"LX Deadzone");
	ui.Tweak_Deadzone_LY->SetName(L"LY Deadzone");
	ui.Tweak_Deadzone_RX->SetName(L"RX Deadzone");
	ui.Tweak_Deadzone_RY->SetName(L"RY Deadzone");
	ui.Tweak_Deadzone_TL->SetName(L"TL Deadzone");
	ui.Tweak_Deadzone_TR->SetName(L"TR Deadzone");
	ui.Tweak_MouseSpeed_X->SetName(L"Mouse Speed X");
	ui.Tweak_MouseSpeed_Y->SetName(L"Mouse Speed Y");
}

void LivingRoomTool::InitConnections()
{
	// Device list
	connect(ui.List_Devices, &QListWidget::itemSelectionChanged, this, &LivingRoomTool::OnDeviceSelectionChanged);
	connect(ui.Btn_RefreshDevices, &QPushButton::clicked, this, &LivingRoomTool::OnRefreshDevicesClicked);
	connect(ui.Btn_IdentifyDevice, &QPushButton::clicked, this, &LivingRoomTool::OnIdentifyDeviceClicked);

	// Preset list
	connect(ui.List_Presets, &QListWidget::itemSelectionChanged, this, &LivingRoomTool::OnPresetSelectionChanged);
}

void LivingRoomTool::OnRefreshDevicesClicked()
{
	m_inputProcessor.GetGamepadProcessor().RescanGamepads();

	std::vector<std::wstring> deviceNames;
	m_inputProcessor.GetGamepadProcessor().GetDeviceNames(deviceNames);

	ui.List_Devices->clear();
	for (const std::wstring& name : deviceNames)
	{
		ui.List_Devices->addItem(QString::fromStdWString(name));
	}
}

void LivingRoomTool::OnIdentifyDeviceClicked()
{
	m_inputProcessor.GetGamepadProcessor().IdentifyDeviceByVibrating(ui.List_Devices->currentIndex().row());
}

void LivingRoomTool::OnDeviceSelectionChanged()
{
}

void LivingRoomTool::OnPresetSelectionChanged()
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