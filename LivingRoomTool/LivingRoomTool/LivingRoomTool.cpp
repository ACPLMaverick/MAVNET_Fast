#include "LivingRoomTool.h"

LivingRoomTool::LivingRoomTool(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_inputProcessor.Init();
	OnRefreshDevicesClicked();

	DisableDevicePanels();
	DisablePresetEditor();

	InitConnections();
	InitTweakNames();
	InitPresetEditor();
}

LivingRoomTool::~LivingRoomTool()
{
	CleanupPresetEditor();
}

void LivingRoomTool::InitTweakNames()
{
	ui.Tweak_Deadzone_L->SetName(L"L Deadzone");
	ui.Tweak_Deadzone_R->SetName(L"R Deadzone");
	ui.Tweak_Deadzone_TL->SetName(L"LT Deadzone");
	ui.Tweak_Deadzone_TR->SetName(L"RT Deadzone");
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

	// Tweaks
#define LRT_ConnectTweakSlider(_type_, _control_, _property_)						\
	connect(_control_, &QTweakSlider::valueChanged, this,							\
		[this](_type_ newVal)														\
	{																				\
		const int selectedIndex = ui.List_Devices->currentRow();					\
		if (selectedIndex >= 0 && selectedIndex < ui.List_Devices->count())			\
		{																			\
			m_inputProcessor.GetGamepadProcessor().GetGamepadDevice(selectedIndex)	\
				.GetConfig().Set_##_property_(newVal);								\
		}																			\
	})

	LRT_ConnectTweakSlider(float, ui.Tweak_Deadzone_L, deadzoneLeftThumb);
	LRT_ConnectTweakSlider(float, ui.Tweak_Deadzone_R, deadzoneRightThumb);
	LRT_ConnectTweakSlider(float, ui.Tweak_Deadzone_TL, deadzoneLeftTrigger);
	LRT_ConnectTweakSlider(float, ui.Tweak_Deadzone_TR, deadzoneRightTrigger);
	LRT_ConnectTweakSlider(float, ui.Tweak_MouseSpeed_X, mouseSpeedX);
	LRT_ConnectTweakSlider(float, ui.Tweak_MouseSpeed_Y, mouseSpeedY);
#undef LRT_ConnectTweakSlider

#define LRT_ConnectTweakInstrumentationMode(_control_, _value_)							\
	connect(_control_, &QRadioButton::clicked, this,									\
		[this]()																		\
	{																					\
		const size_t selectedIndex = GetGamepadSelectedIndex();							\
		if (selectedIndex != k_invalidIndex)											\
		{																				\
			m_inputProcessor.GetGamepadProcessor().GetGamepadDevice(selectedIndex)		\
				.GetConfig()															\
				.Set_instrumentationMode(_value_);										\
		}																				\
	})

	LRT_ConnectTweakInstrumentationMode(ui.Tweak_Enables_RB_Off, GamepadConfig::InstrumentationMode::kOff);
	LRT_ConnectTweakInstrumentationMode(ui.Tweak_Enables_RB_Desktop, GamepadConfig::InstrumentationMode::kDesktop);
	LRT_ConnectTweakInstrumentationMode(ui.Tweak_Enables_RB_Game, GamepadConfig::InstrumentationMode::kGame);
	LRT_ConnectTweakInstrumentationMode(ui.Tweak_Enables_RB_On, GamepadConfig::InstrumentationMode::kOn);
#undef LRT_ConnectTweakInstrumentationMode

	connect(ui.Btn_SaveTweaks, &QAbstractButton::clicked, this, &LivingRoomTool::OnSaveTweakClicked);
	connect(ui.Btn_RestoreTweaks, &QAbstractButton::clicked, this, &LivingRoomTool::OnRestoreTweakClicked);
	connect(ui.Btn_DefaultTweaks, &QAbstractButton::clicked, this, &LivingRoomTool::OnDefaultsTweakClicked);
}

void LivingRoomTool::InitPresetEditor()
{
	m_presetEditor.Init(
		{
			ui.Cb_Gamepad_RT,
			ui.Cb_Gamepad_RB,
			ui.Cb_Gamepad_LT,
			ui.Cb_Gamepad_LB,
			ui.Cb_Gamepad_View,
			ui.Cb_Gamepad_Menu,
			ui.Cb_Gamepad_RUp,
			ui.Cb_Gamepad_RLeft,
			ui.Cb_Gamepad_RDown,
			ui.Cb_Gamepad_RRight,
			ui.Cb_Gamepad_LDown,
			ui.Cb_Gamepad_LLeft,
			ui.Cb_Gamepad_LRight,
			ui.Cb_Gamepad_LUp,
			ui.Cb_Gamepad_RThumbPress,
			ui.Cb_Gamepad_LThumbPress,
			ui.Cb_Gamepad_LThumbDown,
			ui.Cb_Gamepad_LThumbUp,
			ui.Cb_Gamepad_LThumbLeft,
			ui.Cb_Gamepad_LThumbRight,
			ui.Cb_Gamepad_RThumbUp,
			ui.Cb_Gamepad_RThumbDown,
			ui.Cb_Gamepad_RThumbLeft,
			ui.Cb_Gamepad_RThumbRight,
			ui.List_AdvancedBindings,
			ui.BtnAdvancedAdd,
			ui.BtnAdvancedDuplicate,
			ui.BtnAdvancedEdit,
			ui.BtnAdvancedRemove
		},
		&m_inputProcessor.GetInputPresetManager());
}

void LivingRoomTool::CleanupPresetEditor()
{
	m_presetEditor.Cleanup();
}

void LivingRoomTool::EnableDevicePanels()
{
	SetQLayoutElementsFrozen(ui.VertLayout_Presets, false);
	SetQLayoutElementsFrozen(ui.VertLayout_Tweaks, false);
}

void LivingRoomTool::DisableDevicePanels()
{
	SetQLayoutElementsFrozen(ui.VertLayout_Presets, true);
	SetQLayoutElementsFrozen(ui.VertLayout_Tweaks, true);
}

void LivingRoomTool::EnablePresetEditor()
{
	SetQLayoutElementsFrozen(ui.VertLayout_EditPreset, false);
}

void LivingRoomTool::DisablePresetEditor()
{
	SetQLayoutElementsFrozen(ui.VertLayout_EditPreset, true);
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

void LivingRoomTool::OnSaveTweakClicked()
{
	const size_t selectedIndex = GetGamepadSelectedIndex();
	if (selectedIndex != k_invalidIndex)
	{
		m_inputProcessor.GetGamepadProcessor().GetGamepadDevice(selectedIndex)
			.GetConfig().SaveToFile();
	}
}

void LivingRoomTool::OnRestoreTweakClicked()
{
	const size_t selectedIndex = GetGamepadSelectedIndex();
	if (selectedIndex != k_invalidIndex)
	{
		m_inputProcessor.GetGamepadProcessor().GetGamepadDevice(selectedIndex)
			.GetConfig().LoadFromFile();
		UpdatePanelsForSelectedDevice_Tweaks(selectedIndex);
	}
}

void LivingRoomTool::OnDefaultsTweakClicked()
{
	const size_t selectedIndex = GetGamepadSelectedIndex();
	if (selectedIndex != k_invalidIndex)
	{
		m_inputProcessor.GetGamepadProcessor().GetGamepadDevice(selectedIndex)
			.GetConfig().RestoreDefaults();
		UpdatePanelsForSelectedDevice_Tweaks(selectedIndex);
	}
}

void LivingRoomTool::OnDeviceSelectionChanged()
{
	const size_t selectedIndex = GetGamepadSelectedIndex();
	if (selectedIndex != k_invalidIndex)
	{
		EnableDevicePanels();
		UpdatePanelsForSelectedDevice(selectedIndex);
	}
	else
	{
		DisableDevicePanels();
	}
}

void LivingRoomTool::OnPresetSelectionChanged()
{
	const size_t selectedIndex = GetPresetSelectedIndex();
	if (selectedIndex != k_invalidIndex)
	{
		EnablePresetEditor();
		UpdateEditorForSelectedPreset(selectedIndex);
	}
	else
	{
		DisablePresetEditor();
	}
}

void LivingRoomTool::UpdatePanelsForSelectedDevice(size_t selectedDevice)
{
	UpdatePanelsForSelectedDevice_Presets(selectedDevice);
	UpdatePanelsForSelectedDevice_Tweaks(selectedDevice);
}

void LivingRoomTool::UpdatePanelsForSelectedDevice_Presets(size_t selectedDevice)
{
	m_inputProcessor.GetInputPresetManager().LoadPresets();
	
	ui.List_Presets->clear();

	const size_t presetNum = m_inputProcessor.GetInputPresetManager().GetPresetNum();
	for (size_t i = 0; i < presetNum; ++i)
	{
		const std::string& nameStr = m_inputProcessor.GetInputPresetManager().GetPreset(i).GetName();
		const std::wstring nameWstr(nameStr.begin(), nameStr.end());
		ui.List_Presets->addItem(QString::fromStdWString(nameWstr));
	}
}

void LivingRoomTool::UpdatePanelsForSelectedDevice_Tweaks(size_t selectedDevice)
{
	const GamepadConfig& config = m_inputProcessor.GetGamepadProcessor().GetGamepadDevice(selectedDevice).GetConfig();

	QRadioButton* radioButtons[] =
	{
		ui.Tweak_Enables_RB_Off,
		ui.Tweak_Enables_RB_Desktop,
		ui.Tweak_Enables_RB_Game,
		ui.Tweak_Enables_RB_On
	};

	const size_t selectedModeIdx = static_cast<size_t>(config.Get_instrumentationMode());
	for (size_t i = 0; i < static_cast<size_t>(GamepadConfig::InstrumentationMode::ENUM_SIZE); ++i)
	{
		radioButtons[i]->setChecked(selectedModeIdx == i);
	}

	ui.Tweak_Deadzone_L->SetValue(config.Get_deadzoneLeftThumb());
	ui.Tweak_Deadzone_R->SetValue(config.Get_deadzoneRightThumb());
	ui.Tweak_Deadzone_TL->SetValue(config.Get_deadzoneLeftTrigger());
	ui.Tweak_Deadzone_TR->SetValue(config.Get_deadzoneRightTrigger());
	ui.Tweak_MouseSpeed_X->SetValue(config.Get_mouseSpeedX());
	ui.Tweak_MouseSpeed_Y->SetValue(config.Get_mouseSpeedY());
}

void LivingRoomTool::UpdateEditorForSelectedPreset(size_t selectedPreset)
{
	m_presetEditor.AssignPreset(selectedPreset);
}

void LivingRoomTool::ClearEditor()
{
	m_presetEditor.InvalidatePreset();
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

size_t LivingRoomTool::GetQListSelectedIndex(QListWidget* list)
{
	const int selectedIndex = list->currentRow();
	QList<QListWidgetItem*> selectedItems = list->selectedItems();
	if (selectedItems.size() == 1
		&& selectedIndex >= 0
		&& selectedIndex < list->count())
	{
		return static_cast<size_t>(selectedIndex);
	}
	else
	{
		return k_invalidIndex;
	}
}

size_t LivingRoomTool::GetGamepadSelectedIndex()
{
	return GetQListSelectedIndex(ui.List_Devices);
}

size_t LivingRoomTool::GetPresetSelectedIndex()
{
	return GetQListSelectedIndex(ui.List_Presets);
}
