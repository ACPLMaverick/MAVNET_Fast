#include "FileProcessor.h"
#include "CommandParser.h"
#include "FileUtil.h"

Result FileProcessor::Initialize(const CommandParser * parser)
{
	assert(parser);

	m_parser = parser;
	StripFilters();
	Result err = Initialize_Internal();
	return err;
}

Result FileProcessor::Cleanup()
{
	Result err = Cleanup_Internal();
	m_strippedFilters.clear();
	m_fileList.Clear();
	m_parser = nullptr;
	return err;
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
	for (const std::string& filter : m_parser->GetFilters())
	{
		std::string stripped;
		FileUtil::StripString(filter, asterisk, stripped);
		m_strippedFilters.push_back(stripped);
	}
}

void FileProcessor::PerformDiff(const FileList & other)
{
}
