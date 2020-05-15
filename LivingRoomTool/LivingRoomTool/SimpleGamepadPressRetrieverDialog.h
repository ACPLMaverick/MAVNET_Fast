#pragma once

#include <QWidget>
#include <QtWidgets\qmessagebox.h>
#include <qtimer.h>

#include "GamepadState.h"

class GamepadDevice;

class SimpleGamepadPressRetrieverDialog : public QObject
{
	Q_OBJECT

public:

	SimpleGamepadPressRetrieverDialog(QWidget* baseWidget, const GamepadDevice* device);
	~SimpleGamepadPressRetrieverDialog();

	void OpenAsync();
	void Close();
	GamepadButtons GetResult() const { return m_result; }

signals:
	void resultsAvailable();
	void canceled();

private:

	GamepadButtons GetButtonsWithAccountForDeadzones();
	static inline GamepadButtons NegateButtons(GamepadButtons buttons, GamepadButtons buttonsToNegate);

	void TimerTick();
	void OnMessageBoxCanceled();

	
	const GamepadDevice* m_device;

	QMessageBox m_messageBox;
	QTimer m_timer;
	QMetaObject::Connection m_timerConnection;
	GamepadButtons m_result;
	bool m_bPressStarted;

	static const int k_timerIntervalMs = 8;
};

