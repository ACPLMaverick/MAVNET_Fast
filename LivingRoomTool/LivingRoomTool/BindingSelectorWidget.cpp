#include "BindingSelectorWidget.h"

#include "InputAction.h"

BindingSelectorWidget::BindingSelectorWidget(QWidget *parent)
	: QWidget(parent)
{
	m_ui.setupUi(this);

	connect(m_ui.Btn_Edit, &QPushButton::clicked, this, &BindingSelectorWidget::OnEditClicked);
	void (QComboBox::* indexChangedSignal)(int) = &QComboBox::currentIndexChanged;
	connect(m_ui.Cb_Select, indexChangedSignal, this, &BindingSelectorWidget::OnComboBoxIndexChanged);

	m_messageBox.setModal(true);
	m_messageBox.setIcon(QMessageBox::Icon::Question);
	m_messageBox.setStandardButtons(QMessageBox::StandardButton::Cancel);
	m_messageBox.button(QMessageBox::StandardButton::Cancel)->setFocus();
	m_messageBox.setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	m_messageBox.setText("Press a key or mouse button.");
	connect(&m_messageBox, &QMessageBox::close, this, &BindingSelectorWidget::OnMessageBoxCanceled);

	m_timerConnection = connect(&m_timer, &QTimer::timeout, this, &BindingSelectorWidget::TimerTick);
}

BindingSelectorWidget::~BindingSelectorWidget()
{
	disconnect(m_timerConnection);
}

int BindingSelectorWidget::GetIndex() const
{
	return m_ui.Cb_Select->currentIndex();
}

void BindingSelectorWidget::SetIndex(int newIndex)
{
	m_ui.Cb_Select->setCurrentIndex(newIndex);
}

void BindingSelectorWidget::TimerTick()
{
	if (m_keypressRetriever.Tick())
	{
		const InputActionKey desiredKey = m_keypressRetriever.RetrieveAndCleanup();
		m_messageBox.close();
		m_timer.stop();

		if (desiredKey != InputActionKey::kNone)
		{
			SetIndex(static_cast<int>(desiredKey));
		}
	}
}

void BindingSelectorWidget::OnEditClicked()
{
	m_keypressRetriever.Init();
	m_timer.start(k_timerIntervalMs);
	m_messageBox.open();
}

void BindingSelectorWidget::OnMessageBoxCanceled()
{
	m_messageBox.close();
	m_timer.stop();
	m_keypressRetriever.RetrieveAndCleanup();
}

void BindingSelectorWidget::OnComboBoxIndexChanged(int newIndex)
{
	indexChanged(newIndex);
}
