#include "SimpleConfirmationDialog.h"

#include <QtWidgets/QMessageBox.h>
#include <QtWidgets/QAbstractButton.h>

bool SimpleConfirmationDialog::Open(const std::string& a_message)
{
	QMessageBox messageBox;
	PrepareCommon(messageBox);
	messageBox.setText(QString::fromStdString(a_message));
	return messageBox.exec() == QMessageBox::StandardButton::Yes;
}

bool SimpleConfirmationDialog::Open(const std::wstring& a_message)
{
	QMessageBox messageBox;
	PrepareCommon(messageBox);
	messageBox.setText(QString::fromStdWString(a_message));
	return messageBox.exec() == QMessageBox::StandardButton::Yes;
}

void SimpleConfirmationDialog::PrepareCommon(QMessageBox& messageBox)
{
	messageBox.setModal(true);
	messageBox.setIcon(QMessageBox::Icon::Question);
	messageBox.setStandardButtons(QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);
	messageBox.button(QMessageBox::StandardButton::Yes)->setFocus();
}
