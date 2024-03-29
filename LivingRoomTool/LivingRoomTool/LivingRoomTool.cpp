#include "LivingRoomTool.h"

#include "SimpleTextInputDialog.h"
#include "SimpleConfirmationDialog.h"

#include <qmenu.h>
#include <qevent.h>

LivingRoomTool::LivingRoomTool(QWidget* a_parent)
	: QMainWindow(a_parent)
	, m_inputProcessor(reinterpret_cast<InputProcessor::WindowHandle>(winId()))
	, m_trayIcon(this)
	, m_bIsClosing(false)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

	m_inputProcessor.Init();

	DisableDevicePanels();
	DisablePresetEditor();

	InitTrayWindow();
	InitConnections();
	InitTweaks();
	InitPresetEditor();
}

LivingRoomTool::~LivingRoomTool()
{
	CleanupPresetEditor();
}

void LivingRoomTool::PostInit()
{
	OnRefreshDevicesClicked();
}

const GamepadDevice* LivingRoomTool::GetCurrentlySelectedDevice()
{
	const size_t selectedIndex = GetGamepadSelectedIndex();
	const size_t gamepadsNum = m_inputProcessor.GetGamepadProcessor().GetDevicesNum();
	if (selectedIndex < gamepadsNum)
	{
		return &m_inputProcessor.GetGamepadProcessor().GetGamepadDevice(selectedIndex);
	}
	else
	{
		return nullptr;
	}
}

void LivingRoomTool::closeEvent(QCloseEvent* a_event)
{
	if (m_bIsClosing)
	{
		m_trayIcon.hide();
		a_event->accept();
	}
	else
	{
		hide();
		a_event->ignore();
	}
}

void LivingRoomTool::hideEvent(QHideEvent* a_event)
{
	hide();
	a_event->accept();
}

void LivingRoomTool::InitTrayWindow()
{
	m_trayIcon.setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_ComputerIcon));
	m_trayIcon.setToolTip("Living Room Tool");

	QMenu* menu = new QMenu(this);
	menu->addAction("Restore", this,
		[this]()
		{
			showNormal();
			setFocus();
		});
	menu->addAction("Exit", this,
		[this]() 
		{ 
			m_bIsClosing = true;
			close(); 
		});

	connect(&m_trayIcon, &QSystemTrayIcon::activated, this, &LivingRoomTool::OnTrayIconClicked);

	m_trayIcon.setContextMenu(menu);
	m_trayIcon.show();
}

void LivingRoomTool::InitTweaks()
{
	ui.Tweak_Deadzone_L->SetName(L"L Deadzone");
	ui.Tweak_Deadzone_R->SetName(L"R Deadzone");
	ui.Tweak_Deadzone_TL->SetName(L"LT Deadzone");
	ui.Tweak_Deadzone_TR->SetName(L"RT Deadzone");
	ui.Tweak_MouseSpeed_X->SetName(L"Mouse Speed X");
	ui.Tweak_MouseSpeed_X->SetMaxValue(300);
	ui.Tweak_MouseSpeed_Y->SetName(L"Mouse Speed Y");
	ui.Tweak_MouseSpeed_Y->SetMaxValue(300);
	ui.Tweak_MouseScroll->SetName(L"Mouse Speed Scroll");
	ui.Tweak_MouseScroll->SetMaxValue(10);
}

void LivingRoomTool::InitConnections()
{
	// Device list
	connect(ui.List_Devices, &QListWidget::itemSelectionChanged, this, &LivingRoomTool::OnDeviceSelectionChanged);
	connect(ui.Btn_RefreshDevices, &QPushButton::clicked, this, &LivingRoomTool::OnRefreshDevicesClicked);
	connect(ui.Btn_IdentifyDevice, &QPushButton::clicked, this, &LivingRoomTool::OnIdentifyDeviceClicked);

	// Preset list
	connect(ui.List_Presets, &QListWidget::itemSelectionChanged, this, &LivingRoomTool::OnPresetSelectionChanged);
	connect(ui.Btn_DuplicatePreset, &QPushButton::clicked, this, &LivingRoomTool::OnDuplicatePresetClicked);
	connect(ui.Btn_RemovePreset, &QPushButton::clicked, this, &LivingRoomTool::OnRemovePresetClicked);
	connect(ui.Btn_RenamePreset, &QPushButton::clicked, this, &LivingRoomTool::OnRenamePresetClicked);
	connect(ui.Btn_SavePreset, &QPushButton::clicked, this, &LivingRoomTool::OnSavePresetClicked);
	connect(ui.Btn_RestorePreset, &QPushButton::clicked, this, &LivingRoomTool::OnRestorePresetClicked);

	// Tweaks
#define LRT_ConnectTweakSlider(_type_, _control_, _property_)						\
	connect(_control_, &QTweakSlider::valueChanged_##_type_, this,					\
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
	LRT_ConnectTweakSlider(int32_t, ui.Tweak_MouseSpeed_X, mouseSpeedX);
	LRT_ConnectTweakSlider(int32_t, ui.Tweak_MouseSpeed_Y, mouseSpeedY);
	LRT_ConnectTweakSlider(int32_t, ui.Tweak_MouseScroll, mouseSpeedScroll);
#undef LRT_ConnectTweakSlider

#define LRT_ConnectTweakInstrumentationMode(_control_, _value_)							\
	connect(_control_, &QRadioButton::clicked, this,									\
		[this]()																		\
	{																					\
		const size_t selectedIndex = GetGamepadSelectedIndex();							\
		if (selectedIndex != LRT_QTHelper::k_invalidIndex)								\
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
			ui.Bs_Gamepad_RT,
			ui.Bs_Gamepad_RB,
			ui.Bs_Gamepad_LT,
			ui.Bs_Gamepad_LB,
			ui.Bs_Gamepad_View,
			ui.Bs_Gamepad_Menu,
			ui.Bs_Gamepad_RUp,
			ui.Bs_Gamepad_RLeft,
			ui.Bs_Gamepad_RDown,
			ui.Bs_Gamepad_RRight,
			ui.Bs_Gamepad_LDown,
			ui.Bs_Gamepad_LLeft,
			ui.Bs_Gamepad_LRight,
			ui.Bs_Gamepad_LUp,
			ui.Bs_Gamepad_RThumbPress,
			ui.Bs_Gamepad_LThumbPress,
			ui.Bs_Gamepad_LThumbDown,
			ui.Bs_Gamepad_LThumbUp,
			ui.Bs_Gamepad_LThumbLeft,
			ui.Bs_Gamepad_LThumbRight,
			ui.Bs_Gamepad_RThumbUp,
			ui.Bs_Gamepad_RThumbDown,
			ui.Bs_Gamepad_RThumbLeft,
			ui.Bs_Gamepad_RThumbRight,
			ui.List_AdvancedBindings,
			ui.BtnAdvancedAdd,
			ui.BtnAdvancedDuplicate,
			ui.BtnAdvancedEdit,
			ui.BtnAdvancedRemove
		},
		&m_inputProcessor.GetInputPresetManager(), this);
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
	ui.List_Presets->clearSelection();
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

void LivingRoomTool::OnDuplicatePresetClicked()
{
	size_t presetIndex = GetPresetSelectedIndex();
	if (presetIndex == LRT_QTHelper::k_invalidIndex)
	{
		return;
	}

	std::string name;
	if (AskForPresetNameWithExistenceCheck(
		presetIndex,
		name,
		L"Please name a duplicated preset",
		L"Such name already exists. Try again?",
		"_Copy") == false)
	{
		return;
	}

	m_inputProcessor.GetInputPresetManager().DuplicatePreset(presetIndex, name);
	UpdatePanels_Presets();
}

void LivingRoomTool::OnRemovePresetClicked()
{
	size_t presetIndex = GetPresetSelectedIndex();
	if (presetIndex == LRT_QTHelper::k_invalidIndex)
	{
		return;
	}

	const std::string message = "Do you want to remove a \""
		+ m_inputProcessor.GetInputPresetManager().GetPreset(presetIndex).GetName()
		+ "\" preset?";
	if (SimpleConfirmationDialog::Open(message) == false)
	{
		return;
	}

	m_inputProcessor.GetInputPresetManager().RemovePreset(presetIndex);
	UpdatePanels_Presets();
	m_presetEditor.InvalidatePreset();
}

void LivingRoomTool::OnRenamePresetClicked()
{
	size_t presetIndex = GetPresetSelectedIndex();
	if (presetIndex == LRT_QTHelper::k_invalidIndex)
	{
		return;
	}

	std::string name;
	if (AskForPresetNameWithExistenceCheck(
		presetIndex,
		name,
		L"Please give a new name",
		L"Such name already exists. Try again?") == false)
	{
		return;
	}

	m_inputProcessor.GetInputPresetManager().RenamePreset(presetIndex, name);
	UpdatePanels_Presets();
}

void LivingRoomTool::OnSavePresetClicked()
{
	size_t presetIndex = GetPresetSelectedIndex();
	if (presetIndex == LRT_QTHelper::k_invalidIndex)
	{
		return;
	}

	m_inputProcessor.GetInputPresetManager().SavePreset(presetIndex);
}

void LivingRoomTool::OnRestorePresetClicked()
{
	size_t presetIndex = GetPresetSelectedIndex();
	if (presetIndex == LRT_QTHelper::k_invalidIndex)
	{
		return;
	}

	const std::string message = "Do you want to restore a \""
		+ m_inputProcessor.GetInputPresetManager().GetPreset(presetIndex).GetName()
		+ "\" preset?";
	if (SimpleConfirmationDialog::Open(message) == false)
	{
		return;
	}

	m_inputProcessor.GetInputPresetManager().RestorePreset(presetIndex);
	UpdateSelectedPreset(presetIndex);
}

void LivingRoomTool::OnSaveTweakClicked()
{
	const size_t selectedIndex = GetGamepadSelectedIndex();
	if (selectedIndex != LRT_QTHelper::k_invalidIndex)
	{
		m_inputProcessor.GetGamepadProcessor().GetGamepadDevice(selectedIndex)
			.GetConfig().SaveToFile();
	}
}

void LivingRoomTool::OnRestoreTweakClicked()
{
	const size_t selectedIndex = GetGamepadSelectedIndex();
	if (selectedIndex != LRT_QTHelper::k_invalidIndex)
	{
		m_inputProcessor.GetGamepadProcessor().GetGamepadDevice(selectedIndex)
			.GetConfig().LoadFromFile();
		UpdatePanelsForSelectedDevice_Tweaks(selectedIndex);
	}
}

void LivingRoomTool::OnDefaultsTweakClicked()
{
	const size_t selectedIndex = GetGamepadSelectedIndex();
	if (selectedIndex != LRT_QTHelper::k_invalidIndex)
	{
		m_inputProcessor.GetGamepadProcessor().GetGamepadDevice(selectedIndex)
			.GetConfig().RestoreDefaults();
		UpdatePanelsForSelectedDevice_Tweaks(selectedIndex);
	}
}

void LivingRoomTool::OnDeviceSelectionChanged()
{
	const size_t selectedIndex = GetGamepadSelectedIndex();
	if (selectedIndex != LRT_QTHelper::k_invalidIndex)
	{
		m_inputProcessor.GetInputPresetManager().LoadPresets();

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

	UpdatePresetButtonAvailabilityForSelectedPreset(selectedIndex);

	if (selectedIndex != LRT_QTHelper::k_invalidIndex)
	{
		EnablePresetEditor();
		UpdateSelectedPreset(selectedIndex);
	}
	else
	{
		m_presetEditor.InvalidatePreset();
		DisablePresetEditor();
	}
}

void LivingRoomTool::OnTrayIconClicked(QSystemTrayIcon::ActivationReason a_activationReason)
{
	switch (a_activationReason)
	{
	case QSystemTrayIcon::DoubleClick:
		showNormal();
		break;
	case QSystemTrayIcon::Context:
	case QSystemTrayIcon::Trigger:
	{
		QPoint appearPos = QCursor::pos();
		const QRect& popupRect = m_trayIcon.contextMenu()->rect();
		appearPos.setX(appearPos.x() - popupRect.width());
		appearPos.setY(appearPos.y() - popupRect.height());
		m_trayIcon.contextMenu()->popup(appearPos);
	}
		break;
	default:
		break;
	}
}

void LivingRoomTool::UpdatePanelsForSelectedDevice(size_t a_selectedDevice)
{
	UpdatePanels_Presets();
	UpdatePanelsForSelectedDevice_Tweaks(a_selectedDevice);
}

void LivingRoomTool::UpdatePanels_Presets()
{
	ui.List_Presets->clear();

	const size_t presetNum = m_inputProcessor.GetInputPresetManager().GetPresetNum();
	for (size_t i = 0; i < presetNum; ++i)
	{
		const std::string& nameStr = m_inputProcessor.GetInputPresetManager().GetPreset(i).GetName();
		const std::wstring nameWstr(nameStr.begin(), nameStr.end());
		ui.List_Presets->addItem(QString::fromStdWString(nameWstr));
	}
}

void LivingRoomTool::UpdatePanelsForSelectedDevice_Tweaks(size_t a_selectedDevice)
{
	const GamepadConfig& config = m_inputProcessor.GetGamepadProcessor().GetGamepadDevice(a_selectedDevice).GetConfig();

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
	ui.Tweak_MouseScroll->SetValue(config.Get_mouseSpeedScroll());
}

void LivingRoomTool::UpdateSelectedPreset(size_t a_selectedPreset)
{
	m_presetEditor.AssignPreset(a_selectedPreset);
	m_inputProcessor.AssignPreset(a_selectedPreset);
}

void LivingRoomTool::UpdatePresetButtonAvailabilityForSelectedPreset(size_t a_selectedPreset)
{
	if (a_selectedPreset == LRT_QTHelper::k_invalidIndex
		|| m_inputProcessor.GetInputPresetManager().GetPreset(a_selectedPreset).IsDefault())
	{
		ui.Btn_RemovePreset->setEnabled(false);
		ui.Btn_RenamePreset->setEnabled(false);
	}
	else
	{
		ui.Btn_RemovePreset->setEnabled(true);
		ui.Btn_RenamePreset->setEnabled(true);
	}
}

void LivingRoomTool::ClearSelectedPreset()
{
	m_presetEditor.InvalidatePreset();
	m_inputProcessor.InvalidatePreset();
}

bool LivingRoomTool::AskForPresetNameWithExistenceCheck(
	size_t a_selectedPreset,
	std::string& a_outName,
	const wchar_t* a_askTitle,
	const wchar_t* a_askErrorMessage,
	const char* a_nameSuffix)
{
	const std::string& baseName = m_inputProcessor.GetInputPresetManager().GetPreset(a_selectedPreset).GetName();

	while (true)
	{
		a_outName = baseName;
		if (a_nameSuffix != nullptr)
		{
			a_outName += a_nameSuffix;
		}

		if (SimpleTextInputDialog::Open(a_outName, a_askTitle, this) == false)
		{
			return false;
		}

		if (m_inputProcessor.GetInputPresetManager().FindPresetByName(a_outName))
		{
			if (SimpleConfirmationDialog::Open(a_askErrorMessage) == false)
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}
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

size_t LivingRoomTool::GetGamepadSelectedIndex()
{
	return LRT_QTHelper::GetQListSelectedIndex(ui.List_Devices);
}

size_t LivingRoomTool::GetPresetSelectedIndex()
{
	return LRT_QTHelper::GetQListSelectedIndex(ui.List_Presets);
}
