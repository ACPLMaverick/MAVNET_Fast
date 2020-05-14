#pragma once
#include <qobject.h>

#include "InputActionKeypressRetriever.h"

#include <QWidget>
#include <QtWidgets\qmessagebox.h>
#include <qtimer.h>

class SimpleKeypressRetrieverDialog :
	public QObject
{
	Q_OBJECT

public:

	SimpleKeypressRetrieverDialog();
	~SimpleKeypressRetrieverDialog();

	void OpenAsync(bool bAllowKeyCombinations = false);
	void Close();
	InputActionKey GetResult();
	void GetResults(std::vector<InputActionKey>& outResults);

signals:
	void resultsAvailable();
	void canceled();

private:

	void TimerTick();
	void OnMessageBoxCanceled();

	InputActionKeypressRetriever m_keypressRetriever;
	QMessageBox m_messageBox;
	QTimer m_timer;
	QMetaObject::Connection m_timerConnection;
	std::vector<InputActionKey> m_results;

	static const int k_timerIntervalMs = 8;
};

