#pragma once

#include "Common.h"

class SimpleConfirmationDialog
{
public:

	static bool Open(const std::string& message);
	static bool Open(const std::wstring& message);

private:

	static void PrepareCommon(class QMessageBox& messageBox);
};

