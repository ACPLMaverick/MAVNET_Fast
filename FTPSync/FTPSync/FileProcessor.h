#pragma once

#include "FTPSync.h"
#include "FileList.h"
#include "Error.h"

class CommandParser;

class FileProcessor
{
public:

	Result Initialize(const CommandParser* parser);
	Result Cleanup();

	Result SyncTo(const FileList& other);

	inline const FileList& GetFileList() { return m_fileList; }

protected:

	virtual Result Initialize_Internal() = 0;
	virtual Result Cleanup_Internal() = 0;
	virtual Result PerformSync() = 0;

	void StripFilters();
	void PerformDiff(const FileList& other);

	FileList m_fileList;
	std::vector<std::string> m_strippedFilters;
	const CommandParser* m_parser = nullptr;
};