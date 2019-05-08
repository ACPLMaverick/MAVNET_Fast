#include "FileProcessorRemote.h"

#include "CommandParser.h"
#include "FTPConnection.h"
#include "MessagePrinter.h"

void FileProcessorRemote::GetPathFromParser(const CommandParser * parser, std::string& outPath)
{
	outPath = parser->GetRemotePath();
}

FileInterface * FileProcessorRemote::GetLocalFileInterface()
{
	return m_objects.m_ftpConnection;
}

AppState FileProcessorRemote::GetThisAppState()
{
	return AppState::GatheringRemoteFilelist;
}

AppState FileProcessorRemote::GetSyncAppState()
{
	return AppState::SyncPush;
}

bool FileProcessorRemote::PerformFileAdd(const File & file)
{
	return m_objects.m_ftpConnection->PushFile(file.Name, file.Name);
}

bool FileProcessorRemote::PerformFileRemove(const File & file)
{
	return m_objects.m_ftpConnection->RemoveFile(file.Name);
}
