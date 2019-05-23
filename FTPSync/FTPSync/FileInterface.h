#pragma once

#include "FTPSync.h"

class FileList;

class FileInterface
{
public:

	virtual bool PushDirectory() = 0;
	virtual bool PopDirectory() = 0;
	virtual bool ChangeDirectory(const std::string& relativeDirectory) = 0;

	virtual bool RemoveFile(const std::string& path) = 0;
	virtual bool IsDirectoryExist(const std::string& path) = 0;
	virtual bool GetFilesInDirectory(const std::string& directory, const std::vector<std::string>& filters, FileList& outFileList) = 0;

	static void SplitString(const std::string& inString, char delim, std::vector<std::string>& outParts)
	{
		std::stringstream ss;
		ss << inString;

		std::string temp;

		while (std::getline(ss, temp, delim))
		{
			outParts.push_back(temp);
		}
	}

	static void StripString(const std::string& inString, char delim, std::string& outString)
	{
		std::vector<std::string> parts;
		SplitString(inString, delim, parts);

		outString.clear();

		for (const std::string& part : parts)
		{
			outString += part;
		}
	}

protected:
	std::stack<std::string> m_directoryStack;
};