#include "FileProcessor.h"
#include "CommandParser.h"
#include "FileInterface.h"

Result FileProcessor::Initialize(const InitObjects& objects)
{
	m_objects = objects;
	m_objects.Check();

	StripFilters();

	FileInterface* fileInterface = GetLocalFileInterface();
	std::string modsDirectory = m_objects.m_parser->GetLocalPath();

	if (!fileInterface->IsDirectoryExist(modsDirectory))
	{
		return Result::NoLocalDir;
	}

	if (!fileInterface->PushDirectory())
	{
		return Result::Unknown;
	}

	if (!fileInterface->ChangeDirectory(modsDirectory))
	{
		return Result::Unknown;
	}

	if (!fileInterface->GetFilesInDirectory(".", m_strippedFilters, m_fileList))
	{
		return Result::Unknown;
	}

	return Result::OK;
}

Result FileProcessor::Cleanup()
{
	GetLocalFileInterface()->PopDirectory();
	m_strippedFilters.clear();
	m_fileList.Clear();
	m_objects.Clear();
	return Result::OK;
}

Result FileProcessor::SyncTo(const FileList & other)
{
	PerformDiff(other);
	return PerformSync();
}

void FileProcessor::StripFilters()
{
	// Remove unnecessary asterisks which would conflict with filter applying algorithm.

	static const char asterisk = '*';

	std::vector<std::string> filterParts;
	for (const std::string& filter : m_objects.m_parser->GetFilters())
	{
		std::string stripped;
		FileInterface::StripString(filter, asterisk, stripped);
		m_strippedFilters.push_back(stripped);
	}
}

void FileProcessor::PerformDiff(const FileList & other)
{
}
