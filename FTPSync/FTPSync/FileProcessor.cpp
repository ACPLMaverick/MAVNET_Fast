#include "FileProcessor.h"

#include "CommandParser.h"
#include "FileInterface.h"
#include "MessagePrinter.h"

Result FileProcessor::Initialize(const InitObjects& objects)
{
	m_objects = objects;
	m_objects.Check();

	StripFilters();

	FileInterface* fileInterface = GetLocalFileInterface();
	std::string modsDirectory = "";
	GetPathFromParser(m_objects.m_parser, modsDirectory);

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

	MessagePrinter::PrintAppState(GetThisAppState());

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
	MessagePrinter::PrintAppState(AppState::Diff);
	if (!PerformDiff(other))
	{
		MessagePrinter::PrintAppState(AppState::NothingToDo);
		return Result::OK;
	}

	MessagePrinter::PrintAppState(GetSyncAppState());
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

bool FileProcessor::PerformDiff(const FileList & other)
{
	bool bIsAnyDiff = false;

	std::vector<bool> addFlags;
	addFlags.resize(other.GetFileNum(), true);

	for (size_t i = 0; i < m_fileList.GetFileNum(); ++i)
	{
		File& myFile = m_fileList[i];
		myFile.Status = FileStatus::ToRemove;
		for (size_t j = 0; j < other.GetFileNum(); ++j)
		{
			const File& otherFile = other[j];
			if (myFile.Name.compare(otherFile.Name) == 0)
			{
				addFlags[j] = false;
				myFile.Status = FileStatus::OK;
				break;
			}
		}

		if (myFile.Status != FileStatus::OK)
		{
			bIsAnyDiff = true;
		}
	}

	for (size_t i = 0; i < addFlags.size(); ++i)
	{
		if (addFlags[i])
		{
			File& newFile = m_fileList.Append();
			newFile.Name = other[i].Name;
			newFile.Status = FileStatus::ToAdd;

			bIsAnyDiff = true;
		}
	}

	return bIsAnyDiff;
}

Result FileProcessor::PerformSync()
{
	bool bHasAnySucceeded = false;
	bool bHasAnyFailed = false;

	for (size_t i = 0; i < m_fileList.GetFileNum(); ++i)
	{
		const File& file = m_fileList[i];
		switch (file.Status)
		{
		case FileStatus::ToAdd:
			MessagePrinter::PrintFile(file);
			if (!PerformFileAdd(file))
			{
				MessagePrinter::PrintResult(Result::Transfer);
				bHasAnyFailed = true;
			}
			else
			{
				bHasAnySucceeded = true;
			}
			break;
		case FileStatus::ToRemove:
			MessagePrinter::PrintFile(file);
			if (!PerformFileRemove(file))
			{
				MessagePrinter::PrintResult(Result::Transfer);
				bHasAnyFailed = true;
			}
			else
			{
				bHasAnySucceeded = true;
			}
			break;
		case FileStatus::Unknown:
			FTPS_Assert(false);
		case FileStatus::OK:
		default:
			continue;
		}
	}
	return bHasAnySucceeded ? (bHasAnyFailed ? Result::OKPartial : Result::OK) : Result::Transfer;
}
