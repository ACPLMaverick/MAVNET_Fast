#include "FileProcessorLocal.h"

#include "CommandParser.h"
#include "FileUtil.h"

Result FileProcessorLocal::Initialize_Internal()
{
	std::string modsDirectory = m_parser->GetLocalPath();
	if (!FileUtil::IsDirectoryExist(modsDirectory))
	{
		return Result::NoLocalDir;
	}

	if (!FileUtil::GetFilesInDirectory(modsDirectory, m_strippedFilters, m_fileList))
	{
		return Result::Unknown;
	}

	return Result::OK;
}

Result FileProcessorLocal::Cleanup_Internal()
{
	return Result::OK;
}

Result FileProcessorLocal::PerformSync()
{
	return Result();
}
