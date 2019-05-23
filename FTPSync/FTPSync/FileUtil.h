#pragma once

#include "FileInterface.h"

class FileUtil : public FileInterface
{
public:

	virtual bool PushDirectory() override;
	virtual bool PopDirectory() override;
	virtual bool ChangeDirectory(const std::string& relativeDirectory) override;

	virtual bool RemoveFile(const std::string& path) override;
	virtual bool IsDirectoryExist(const std::string& path) override;
	virtual bool GetFilesInDirectory(const std::string& directory, const std::vector<std::string>& filters, FileList& outFileList) override;


private:

	void ProcessFileInDirectory(const std::string& directory, const std::string& fileName, const std::vector<std::string>& filters, FileList& outFileList);

public:
#ifdef FTPS_PLATFORM_WIN32
	static const char DIRECTORY_DELIM = '\\';
#elif defined(FTPS_PLATFORM_LINUX)
	static const char DIRECTORY_DELIM = '/';
#endif
};