#pragma once

#include "ui_SimpleTextInputDialog.h"

#include <QtWidgets/QDialog.h>
#include <QtWidgets/QPushButton>

class SimpleTextInputDialog : public QDialog
{
	Q_OBJECT

public:

	static bool Open(std::wstring& refText, const std::wstring& title = L"", QWidget* parent = Q_NULLPTR);
	static bool Open(std::string& refText, const std::wstring& title = L"", QWidget* parent = Q_NULLPTR);

private:

	SimpleTextInputDialog(std::wstring& refText, const std::wstring& title, QWidget* parent);
	SimpleTextInputDialog(std::string& refText, const std::wstring& title, QWidget* parent);
	~SimpleTextInputDialog();
	SimpleTextInputDialog(const SimpleTextInputDialog& other) = delete;
	SimpleTextInputDialog& operator=(const SimpleTextInputDialog& other) = delete;

	void Init(const std::wstring& title);

	Ui_SimpleTextInputDialog m_ui;
};

