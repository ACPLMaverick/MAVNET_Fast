#include "FilePath.h"

#include <Windows.h>
#include <ShlObj_core.h>

const std::wstring & FilePath::GetBaseDirectory()
{
	static std::wstring directory;

	if (directory.empty())
	{
		wchar_t* pathPtr(nullptr);
		LRT_CheckHR(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, NULL, &pathPtr));
		LRT_Assert(pathPtr != nullptr);
		if (pathPtr != nullptr)
		{
			directory = std::wstring(pathPtr) + L"\\LivingRoomTool";
			CoTaskMemFree(pathPtr);
		}
	}
	return directory;
}

void FilePath::MakeSureDirectoryExists(const std::wstring & a_directory)
{
	// Do not proceed if there is only drive leter left.
	if (a_directory.length() <= 3)
	{
		return;
	}

	// Simply try to create that directory without caring about errors.
	BOOL ret = CreateDirectoryW(a_directory.c_str(), NULL);
	if (ret == FALSE)
	{
		if (GetLastError() == ERROR_PATH_NOT_FOUND)
		{
			// Go backwards until we find a valid directory.
			size_t splitPos = a_directory.find_last_of('\\');
			if (splitPos != std::string::npos)
			{
				std::wstring cutDirectory = a_directory.substr(0, splitPos);
				MakeSureDirectoryExists(cutDirectory);
			}
			// Try to create it again.
			ret = CreateDirectoryW(a_directory.c_str(), NULL);
			LRT_Assert(ret == TRUE);	// We should not fail this time.
		}
	}
}
