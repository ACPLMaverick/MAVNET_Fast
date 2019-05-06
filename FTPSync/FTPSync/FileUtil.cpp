#include "FileUtil.h"

#include "FileList.h"

#ifdef FTPS_PLATFORM_WIN32

#include <Windows.h>

bool FileUtil::PushDirectory()
{
	static const size_t BUF_SIZE(128);
	static char buf[BUF_SIZE] = {};

	GetCurrentDirectory(BUF_SIZE, buf);

	m_directoryStack.push(std::string(buf));

	return true;
}

bool FileUtil::PopDirectory()
{
	if (m_directoryStack.empty())
	{
		return false;
	}

	std::string topDir = m_directoryStack.top();
	m_directoryStack.pop();

	return ChangeDirectory(topDir);
}

bool FileUtil::ChangeDirectory(const std::string & relativeDirectory)
{
	return SetCurrentDirectory(relativeDirectory.c_str());
}

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

	std::string dirAsterisk = directory.back() != '*' ? directory + DIRECTORY_DELIM + '*' : directory;

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

#elif FTPS_PLATFORM_LINUX

#error Implement FileUtil for Linux!

bool FileUtil::PushDirectory()
{
	return false;
}

bool FileUtil::PopDirectory()
{
	if (m_directoryStack.empty())
	{
		return false;
	}

	std::string topDir = m_directoryStack.top();
	m_directoryStack.pop();

	return ChangeDirectory(topDir);
}

bool FileUtil::ChangeDirectory(const std::string & relativeDirectory)
{
	return false;
}

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

	if (IsDirectoryExist(directory + DIRECTORY_DELIM + fileName))
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
				listFile.Status = FileStatus::Unknown;

				return;
			}
		}
	}
}
