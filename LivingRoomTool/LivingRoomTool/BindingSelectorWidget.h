#pragma once

#include "ui_BindingSelectorWidget.h"

#include "SimpleKeypressRetrieverDialog.h"

enum class InputActionKey : uint8_t;

class BindingSelectorWidget : public QWidget
{
	Q_OBJECT

public:
	BindingSelectorWidget(QWidget *parent = Q_NULLPTR);
	~BindingSelectorWidget();

	void Fill();
	void Clear();

	int GetIndex() const;
	void SetIndex(int newIndex);

signals:
	void indexChanged(int newIndex);

private:

	void OnEditClicked();
	void OnResultsAvailable();
	void OnComboBoxIndexChanged(int newIndex);

	Ui::BindingSelectorWidget m_ui;
	SimpleKeypressRetrieverDialog m_keypressRetrieverDialog;
};
