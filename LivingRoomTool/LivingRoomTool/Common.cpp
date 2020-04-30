#include "Common.h"

#include <Windows.h>
#include <QtWidgets/QListWidget.h>

#ifndef NDEBUG
//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
void LRT_PrintLastError()
{
	const std::string str = "LRT : ERROR : " + LRT_GetLastErrorAsString();
	if (str.empty() == false)
	{
		OutputDebugStringA(str.c_str());
	}
}

std::string LRT_GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}
#endif

size_t LRT_QTHelper::GetQListSelectedIndex(QListWidget * a_list)
{
	const int selectedIndex = a_list->currentRow();
	QList<QListWidgetItem*> selectedItems = a_list->selectedItems();
	if (selectedItems.size() == 1
		&& selectedIndex >= 0
		&& selectedIndex < a_list->count())
	{
		return static_cast<size_t>(selectedIndex);
	}
	else
	{
		return k_invalidIndex;
	}
}
