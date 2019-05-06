#include "FileProcessorRemote.h"

#include "CommandParser.h"
#include "FTPConnection.h"

FileInterface * FileProcessorRemote::GetLocalFileInterface()
{
	return m_objects.m_ftpConnection;
}

Result FileProcessorRemote::PerformSync()
{
	return Result::NotImplemented;
}
