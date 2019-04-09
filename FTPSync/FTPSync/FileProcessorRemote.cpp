#include "FileProcessorRemote.h"

#include "CommandParser.h"

Result FileProcessorRemote::Initialize_Internal()
{
	// Open connection.
	const std::string ipAddr = m_parser->GetIpAddress();

	// Navigate to modfolder root.

	// Gather filelist.

	return Result::OK;
}

Result FileProcessorRemote::Cleanup_Internal()
{
	// Close connection.


	return Result::OK;
}

Result FileProcessorRemote::PerformSync()
{
	return Result();
}
