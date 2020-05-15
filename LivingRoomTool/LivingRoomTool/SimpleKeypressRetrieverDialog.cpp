#include "SimpleKeypressRetrieverDialog.h"

#include "InputAction.h"

#include <QtWidgets/qpushbutton.h>

SimpleKeypressRetrieverDialog::SimpleKeypressRetrieverDialog(QWidget* a_baseWidget)
	: m_messageBox(a_baseWidget)
{
	m_messageBox.setModal(true);
	m_messageBox.setIcon(QMessageBox::Icon::Question);
	m_messageBox.setStandardButtons(QMessageBox::StandardButton::Cancel);
	m_messageBox.button(QMessageBox::StandardButton::Cancel)->setFocus();
	m_messageBox.setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	connect(&m_messageBox, &QMessageBox::rejected, this, &SimpleKeypressRetrieverDialog::OnMessageBoxCanceled);

	m_timerConnection = connect(&m_timer, &QTimer::timeout, this, &SimpleKeypressRetrieverDialog::TimerTick);
}

SimpleKeypressRetrieverDialog::~SimpleKeypressRetrieverDialog()
{
	disconnect(m_timerConnection);
}

void SimpleKeypressRetrieverDialog::OpenAsync(bool a_bAllowKeyCombinations /*= false*/)
{
	m_results.clear();

	if (a_bAllowKeyCombinations)
	{
		m_messageBox.setText("Press a key, key combination or a mouse button.");
	}
	else
	{
		m_messageBox.setText("Press a key or a mouse button.");
	}

	m_keypressRetriever.Init();
	m_timer.start(k_timerIntervalMs);
	m_messageBox.open();
}

void SimpleKeypressRetrieverDialog::Close()
{
	m_messageBox.close();
	m_timer.stop();
	m_keypressRetriever.RetrieveAndCleanup();
}

InputActionKey SimpleKeypressRetrieverDialog::GetResult() const
{
	return m_results[0];
}

void SimpleKeypressRetrieverDialog::GetResults(std::vector<InputActionKey>& a_outResults) const
{
	a_outResults = m_results;
}

void SimpleKeypressRetrieverDialog::TimerTick()
{
	if (m_keypressRetriever.Tick()
		&& (m_messageBox.button(QMessageBox::StandardButton::Cancel)->isDown() == false))
	{
		m_keypressRetriever.RetrieveAndCleanup(m_results);
		if (m_results.empty())
		{
			m_results.push_back(InputActionKey::kNone);
		}

		m_messageBox.accept();
		m_timer.stop();

		resultsAvailable();
	}
}

void SimpleKeypressRetrieverDialog::OnMessageBoxCanceled()
{
	m_keypressRetriever.RetrieveAndCleanup();
	m_timer.stop();

	canceled();
}
