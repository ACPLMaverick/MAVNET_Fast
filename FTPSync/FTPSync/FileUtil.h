#pragma once

#include "FTPSync.h"

class FileList;

class FileUtil
{
public:

	static bool RemoveFile(const std::string& path);
	static bool IsDirectoryExist(const std::string& path);
	static bool GetFilesInDirectory(const std::string& directory, const std::vector<std::string>& filters, FileList& outFileList);
	static bool ChangeCurrentDirectory(const std::string& relativeDirectory);

	static void ProcessFileInDirectory(const std::string& directory, const std::string& fileName, const std::vector<std::string>& filters, FileList& outFileList);
	static void SplitString(const std::string& inString, char delim, std::vector<std::string>& outParts);
	static void StripString(const std::string& inString, char delim, std::string& outString);
};