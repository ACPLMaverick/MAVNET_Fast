#include "FileProcessorLocal.h"

#include "CommandParser.h"
#include "FileUtil.h"


FileInterface * FileProcessorLocal::GetLocalFileInterface()
{
	return m_objects.m_fileUtil;
}

Result FileProcessorLocal::PerformSync()
{
	return Result::NotImplemented;
}
