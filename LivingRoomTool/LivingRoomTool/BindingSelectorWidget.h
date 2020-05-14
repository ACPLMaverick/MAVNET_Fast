#pragma once

#include "ui_BindingSelectorWidget.h"
#include "InputActionKeypressRetriever.h"

#include <QWidget>
#include <QtWidgets\qmessagebox.h>
#include <qtimer.h>

class BindingSelectorWidget : public QWidget
{
	Q_OBJECT

public:
	BindingSelectorWidget(QWidget *parent = Q_NULLPTR);
	~BindingSelectorWidget();

	template<typename EnumType> void Fill()
	{
		m_ui.Cb_Select->clear();

		static const size_t k_enumSize = magic_enum::enum_count<EnumType>();
		for (size_t i = 0; i < k_enumSize; ++i)
		{
			const std::string_view& str = magic_enum::enum_name(static_cast<EnumType>(i));
			LRT_Assert(str.size() > 0);
			if (str.size() == 0)
			{
				continue;
			}

			m_ui.Cb_Select->addItem(QString::fromUtf8(str.data(), str.size()));
		}

		m_ui.Cb_Select->setCurrentIndex(0);
	}

	int GetIndex() const;
	void SetIndex(int newIndex);

signals:
	void indexChanged(int newIndex);

private:

	void TimerTick();

	void OnEditClicked();
	void OnMessageBoxCanceled();
	void OnComboBoxIndexChanged(int newIndex);

	Ui::BindingSelectorWidget m_ui;
	InputActionKeypressRetriever m_keypressRetriever;
	QMessageBox m_messageBox;
	QTimer m_timer;
	QMetaObject::Connection m_timerConnection;

	static const int k_timerIntervalMs = 8;
};
