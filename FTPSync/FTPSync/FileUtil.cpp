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

#elif defined(FTPS_PLATFORM_LINUX)

#include <unistd.h>
#include <dirent.h>

bool FileUtil::PushDirectory()
{
	static const size_t BUF_SIZE(128);
	static char buf[BUF_SIZE] = {};

	char* retPtr = getcwd(buf, BUF_SIZE);

	bool bIsGood = retPtr == buf;

	if(bIsGood)
	{
		m_directoryStack.push(std::string(buf));
	}

	return bIsGood;
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
	bool ret = false;
	try
	{
		ret = chdir(relativeDirectory.c_str()) == 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << "FileUtil::ChangeDirectory error: "<< e.what() << '\n';
		return false;
	}
	
	return ret;
}

bool FileUtil::RemoveFile(const std::string & path)
{
	bool ret = false;
	try
	{
		ret = unlink(path.c_str()) == 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << "FileUtil::RemoveFile error: " << e.what() << '\n';
		return false;
	}
	
	return ret;
}

bool FileUtil::IsDirectoryExist(const std::string & path)
{
	DIR* dir = opendir(path.c_str());
	if(dir)
	{
		closedir(dir);
		return true;
	}
	else
	{
		return false;
	}
}

bool FileUtil::GetFilesInDirectory(const std::string & directory, const std::vector<std::string>& filters, FileList & outFileList)
{
	dirent** fileListTemp;
	int numEntries = scandir(directory.c_str(), &fileListTemp, NULL, alphasort);
	if(numEntries < 0)
	{
		return false;
	}

	for(int i = 0; i < numEntries; ++i)
	{
		ProcessFileInDirectory(directory, std::string(fileListTemp[i]->d_name), filters, outFileList);
		free(fileListTemp[i]);
	}
	free(fileListTemp);

	return numEntries > 0;
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
