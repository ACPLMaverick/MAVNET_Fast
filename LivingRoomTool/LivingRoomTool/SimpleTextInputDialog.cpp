#include "SimpleTextInputDialog.h"


bool SimpleTextInputDialog::Open(std::wstring & a_refText, const std::wstring & a_title, QWidget * a_parent)
{
	SimpleTextInputDialog dialog(a_refText, a_title, a_parent);
	const bool retVal = dialog.exec() == QDialog::DialogCode::Accepted;
	if (retVal)
	{
		a_refText = dialog.m_ui.textEdit->text().toStdWString();
	}
	return retVal;
}

bool SimpleTextInputDialog::Open(std::string & a_refText, const std::wstring & a_title, QWidget * a_parent)
{
	SimpleTextInputDialog dialog(a_refText, a_title, a_parent);
	const bool retVal = dialog.exec() == QDialog::DialogCode::Accepted;
	if (retVal)
	{
		a_refText = dialog.m_ui.textEdit->text().toStdString();
	}
	return retVal;
}

SimpleTextInputDialog::SimpleTextInputDialog(std::wstring & a_refText, const std::wstring & a_title, QWidget * a_parent)
	: QDialog(a_parent)
{
	Init(a_title);
	m_ui.textEdit->setText(QString::fromStdWString(a_refText));
}

SimpleTextInputDialog::SimpleTextInputDialog(std::string & a_refText, const std::wstring & a_title, QWidget * a_parent)
	: QDialog(a_parent)
{
	Init(a_title);
	m_ui.textEdit->setText(QString::fromStdString(a_refText));
}

SimpleTextInputDialog::~SimpleTextInputDialog()
{
}

void SimpleTextInputDialog::Init(const std::wstring & title)
{
	m_ui.setupUi(this);
	connect(m_ui.okButton, &QPushButton::click, this, &SimpleTextInputDialog::accept);
	connect(m_ui.cancelButton, &QPushButton::click, this, &SimpleTextInputDialog::reject);

	setWindowTitle(QString::fromStdWString(title));

	m_ui.textEdit->setFocus();
}
