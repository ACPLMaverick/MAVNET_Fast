#include "SimpleGamepadPressRetrieverDialog.h"

#include "GamepadDevice.h"

#include <QtWidgets/qabstractbutton.h>

extern bool HACK_g_disallowInstrumentation;

SimpleGamepadPressRetrieverDialog::SimpleGamepadPressRetrieverDialog(QWidget* a_baseWidget, const GamepadDevice* a_device)
	: m_device(a_device)
	, m_bPressStarted(false)
	, m_messageBox(a_baseWidget)
{
	LRT_Assert(m_device != nullptr);

	m_messageBox.setModal(true);
	m_messageBox.setIcon(QMessageBox::Icon::Question);
	m_messageBox.setStandardButtons(QMessageBox::StandardButton::Cancel);
	m_messageBox.button(QMessageBox::StandardButton::Cancel)->setFocus();
	m_messageBox.setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	m_messageBox.setText("Press a gamepad button, axis or any combination of them.");
	connect(&m_messageBox, &QMessageBox::rejected, this, &SimpleGamepadPressRetrieverDialog::OnMessageBoxCanceled);

	m_timerConnection = connect(&m_timer, &QTimer::timeout, this, &SimpleGamepadPressRetrieverDialog::TimerTick);
}

SimpleGamepadPressRetrieverDialog::~SimpleGamepadPressRetrieverDialog()
{
	disconnect(m_timerConnection);
}

void SimpleGamepadPressRetrieverDialog::OpenAsync()
{
	m_bPressStarted = false;
	m_result = GamepadButtons::kNone;
	m_timer.start(k_timerIntervalMs);
	m_messageBox.open();
	HACK_g_disallowInstrumentation = true;
}

void SimpleGamepadPressRetrieverDialog::Close()
{
	m_messageBox.close();
	m_timer.stop();
	HACK_g_disallowInstrumentation = false;
}

GamepadButtons SimpleGamepadPressRetrieverDialog::GetButtonsWithAccountForDeadzones()
{
	const GamepadState& state = m_device->GetState();
	const GamepadConfig& config = m_device->GetConfig();

	GamepadButtons buttons = state.GetCurrentButtons();

	static const float k_fixedDeadzone = 0.5f;

	if ((buttons & GamepadButtons::kLT) != GamepadButtons::kNone)
	{
		if (state.GetLeftTrigger().Axis < k_fixedDeadzone)
		{
			buttons = NegateButtons(buttons, GamepadButtons::kLT);
		}
	}
	if ((buttons & GamepadButtons::kRT) != GamepadButtons::kNone)
	{
		if (state.GetRightTrigger().Axis < k_fixedDeadzone)
		{
			buttons = NegateButtons(buttons, GamepadButtons::kRT);
		}
	}
	if ((buttons & GamepadButtons::kLThumbDown) != GamepadButtons::kNone)
	{
		if (state.GetLeftThumb().AxisY > -k_fixedDeadzone)
		{
			buttons = NegateButtons(buttons, GamepadButtons::kLThumbDown);
		}
	}
	if ((buttons & GamepadButtons::kLThumbUp) != GamepadButtons::kNone)
	{
		if (state.GetLeftThumb().AxisY < k_fixedDeadzone)
		{
			buttons = NegateButtons(buttons, GamepadButtons::kLThumbUp);
		}
	}
	if ((buttons & GamepadButtons::kLThumbLeft) != GamepadButtons::kNone)
	{
		if (state.GetLeftThumb().AxisX > -k_fixedDeadzone)
		{
			buttons = NegateButtons(buttons, GamepadButtons::kLThumbLeft);
		}
	}
	if ((buttons & GamepadButtons::kLThumbRight) != GamepadButtons::kNone)
	{
		if (state.GetLeftThumb().AxisX < k_fixedDeadzone)
		{
			buttons = NegateButtons(buttons, GamepadButtons::kLThumbRight);
		}
	}
	if ((buttons & GamepadButtons::kRThumbDown) != GamepadButtons::kNone)
	{
		if (state.GetRightThumb().AxisY > -k_fixedDeadzone)
		{
			buttons = NegateButtons(buttons, GamepadButtons::kRThumbDown);
		}
	}
	if ((buttons & GamepadButtons::kRThumbUp) != GamepadButtons::kNone)
	{
		if (state.GetRightThumb().AxisY < k_fixedDeadzone)
		{
			buttons = NegateButtons(buttons, GamepadButtons::kRThumbUp);
		}
	}
	if ((buttons & GamepadButtons::kRThumbLeft) != GamepadButtons::kNone)
	{
		if (state.GetRightThumb().AxisX > -k_fixedDeadzone)
		{
			buttons = NegateButtons(buttons, GamepadButtons::kRThumbLeft);
		}
	}
	if ((buttons & GamepadButtons::kRThumbRight) != GamepadButtons::kNone)
	{
		if (state.GetRightThumb().AxisX < k_fixedDeadzone)
		{
			buttons = NegateButtons(buttons, GamepadButtons::kRThumbRight);
		}
	}

	return buttons;
}

inline GamepadButtons SimpleGamepadPressRetrieverDialog::NegateButtons(GamepadButtons buttons, GamepadButtons buttonsToNegate)
{
	return buttons & static_cast<GamepadButtons>(~static_cast<uint32_t>(buttonsToNegate));
}

void SimpleGamepadPressRetrieverDialog::TimerTick()
{
	GamepadButtons currentButtons = GetButtonsWithAccountForDeadzones();

	m_result |= currentButtons;

	if (m_bPressStarted == false && currentButtons != GamepadButtons::kNone)
	{
		m_bPressStarted = true;
	}
	else if (m_bPressStarted && currentButtons == GamepadButtons::kNone)
	{
		m_timer.stop();
		m_messageBox.accept();
		resultsAvailable();
		HACK_g_disallowInstrumentation = false;
	}
}

void SimpleGamepadPressRetrieverDialog::OnMessageBoxCanceled()
{
	m_timer.stop();
	canceled();
	HACK_g_disallowInstrumentation = false;
}
