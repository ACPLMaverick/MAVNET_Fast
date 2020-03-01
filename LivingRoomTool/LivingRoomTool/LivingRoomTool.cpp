#include "LivingRoomTool.h"

LivingRoomTool::LivingRoomTool(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	InitVisibilities();
	InitConnections();

	m_inputProcessor.Init();

	OnRefreshDevicesClicked();
}

void LivingRoomTool::InitVisibilities()
{
	SetQLayoutElementsFrozen(ui.VertLayout_Presets, true);
	SetQLayoutElementsFrozen(ui.VertLayout_EditPreset, true);
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
	m_inputProcessor.GetGamepadProcessor().FindGamepads();

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
	m_inputProcessor.GetGamepadProcessor().SelectDevice(static_cast<uint32_t>(ui.List_Devices->currentIndex().row()));
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