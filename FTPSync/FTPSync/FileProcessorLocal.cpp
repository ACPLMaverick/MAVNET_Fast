#include "FileProcessorLocal.h"

#include "CommandParser.h"
#include "FileUtil.h"
#include "MessagePrinter.h"
#include "FTPConnection.h"

void FileProcessorLocal::GetPathFromParser(const CommandParser * parser, std::string& outPath)
{
	outPath = parser->GetLocalPath();
}

FileInterface * FileProcessorLocal::GetLocalFileInterface()
{
	return m_objects.m_fileUtil;
}

AppState FileProcessorLocal::GetThisAppState()
{
	return AppState::GatheringLocalFilelist;
}

AppState FileProcessorLocal::GetSyncAppState()
{
	return AppState::SyncPull;
}

bool FileProcessorLocal::PerformFileAdd(const File & file)
{
	return m_objects.m_ftpConnection->PullFile(file.Name, file.Name);
}

bool FileProcessorLocal::PerformFileRemove(const File & file)
{
	return m_objects.m_fileUtil->RemoveFile(file.Name);
}
