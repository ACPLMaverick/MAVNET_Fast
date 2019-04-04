#include "FileUtil.h"

#include "FileList.h"

#include <sstream>
#include <stack>

#ifdef FTPS_PLATFORM_WIN32

#include <Windows.h>

bool FileUtil::RemoveFile(const std::string & path)
{
	return DeleteFile(path.c_str());
}

bool FileUtil::IsDirectoryExist(const std::string & path)
{
	DWORD attrib = GetFileAttributes(path.c_str());

	if (attrib == INVALID_FILE_ATTRIBUTES)
	{
		assert(false);
		return false;
	}

	if (attrib & FILE_ATTRIBUTE_DIRECTORY)
	{
		return true;
	}

	return false;
}

bool FileUtil::GetFilesInDirectory(const std::string & directory, const std::vector<std::string>& filters, FileList & outFileList)
{
	WIN32_FIND_DATA findData;
	HANDLE hFile = INVALID_HANDLE_VALUE;

	std::string dirAsterisk = directory.back() != '*' ? directory + "\\*" : directory;

	hFile = FindFirstFile(dirAsterisk.c_str(), &findData);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		assert(false);
		return false;
	}

	do
	{
		ProcessFileInDirectory(directory, findData.cFileName, filters, outFileList);

	} while (FindNextFile(hFile, &findData));

	FindClose(hFile);

	return true;
}

bool FileUtil::ChangeCurrentDirectory(const std::string & relativeDirectory)
{
	return SetCurrentDirectory(relativeDirectory.c_str());
}

#elif FTPS_PLATFORM_LINUX

bool FileUtil::RemoveFile(const std::string & path)
{
	return false;
}

bool FileUtil::IsDirectoryExist(const std::string & path)
{
	return false;
}

bool FileUtil::GetFilesInDirectory(const std::string & directory, const std::vector<std::string>& filters, bool bRecursive, FileList & outFileList)
{
	return false;
}

bool FileUtil::ChangeCurrentDirectory(const std::string & relativeDirectory)
{
	return false;
}

#else
#error Not implemented FileUtil.
#endif

void FileUtil::ProcessFileInDirectory(const std::string& directory, const std::string & fileName, const std::vector<std::string>& filters, FileList & outFileList)
{
	if (
		fileName.compare(".") == 0
		|| fileName.compare("..") == 0
		)
	{
		return;
	}

	if (IsDirectoryExist(directory + "\\" + fileName))
	{
		return;
	}

	// Filters.
	if (!filters.empty())
	{
		for (const std::string& filter : filters)
		{
			if (fileName.find(filter) != std::string::npos)
			{
				File& listFile = outFileList.Append();
				listFile.Name = fileName;
				listFile.Status = FileStatus::OK;

				return;
			}
		}
	}
}

void FileUtil::SplitString(const std::string & inString, char delim, std::vector<std::string>& outParts)
{
	std::stringstream ss;
	ss << inString;

	std::string temp;

	while (std::getline(ss, temp, delim))
	{
		outParts.push_back(temp);
	}
}

void FileUtil::StripString(const std::string & inString, char delim, std::string & outString)
{
	std::vector<std::string> parts;
	FileUtil::SplitString(inString, delim, parts);

	outString.clear();

	for (const std::string& part : parts)
	{
		outString += part;
	}
}
