#pragma once

#include "FTPSync.h"
#include "FileList.h"
#include "Error.h"
#include "AppState.h"

class CommandParser;
class FileInterface;
class FTPConnection;
class FileUtil;

class FileProcessor
{
public:

	struct InitObjects
	{
		const CommandParser* m_parser = nullptr;
		FTPConnection* m_ftpConnection = nullptr;
		FileUtil* m_fileUtil = nullptr;

		inline bool Check()
		{
			const bool check = (m_parser != nullptr && m_ftpConnection != nullptr && m_fileUtil != nullptr);
			FTPS_Assert(check);
			return check;
		}

		inline void Clear()
		{
			m_parser = nullptr;
			m_ftpConnection = nullptr;
			m_fileUtil = nullptr;
		}
	};

public:

	Result Initialize(const InitObjects& objects);
	Result Cleanup();

	Result SyncTo(const FileList& other);

	inline const FileList& GetFileList() { return m_fileList; }

protected:

	virtual void GetPathFromParser(const CommandParser* parser, std::string& outPath) = 0;
	virtual FileInterface* GetLocalFileInterface() = 0;
	virtual AppState GetThisAppState() = 0;
	virtual AppState GetSyncAppState() = 0;
	virtual bool PerformFileAdd(const File& file) = 0;
	virtual bool PerformFileRemove(const File& file) = 0;

	void StripFilters();
	bool PerformDiff(const FileList& other);
	Result PerformSync();

	FileList m_fileList;
	std::vector<std::string> m_strippedFilters;
	InitObjects m_objects;
};