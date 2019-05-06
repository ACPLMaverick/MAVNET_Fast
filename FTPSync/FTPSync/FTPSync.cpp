// FTPSync.cpp : Defines the entry point for the application.
//

#include "FTPSync.h"

#include "CommandParser.h"
#include "FTPConnection.h"
#include "MessagePrinter.h"
#include "FileUtil.h"

#include "FileProcessorLocal.h"
#include "FileProcessorRemote.h"

#include "Error.h"

Result Process(const FileProcessor::InitObjects& initObjects, FileProcessor* fpSource, FileProcessor* fpDestination)
{
	assert(fpSource);
	assert(fpDestination);
	
	Result res = fpSource->Initialize(initObjects);
	if (res != Result::OK)
	{
		return res;
	}
	fpDestination->Initialize(initObjects);
	if (res != Result::OK)
	{
		fpSource->Cleanup();
		return res;
	}

	res = fpDestination->SyncTo(fpSource->GetFileList());

	fpDestination->Cleanup();
	fpSource->Cleanup();

	return res;
}

int main(int argc, char* argv[])
{
	CommandParser parser(argc, argv);
	FTPConnection connection(&parser);
	FileUtil fileUtil;

	FileProcessor* fpSource(nullptr);
	FileProcessor* fpDestination(nullptr);

	if (parser.GetMode() == WorkMode::Unknown)
	{
		MessagePrinter::PrintResult(Result::InvalidWorkMode);
		FTPS_Assert(false);
		return -1;
	}
	else if (parser.GetMode() == WorkMode::Pull)
	{
		fpSource = new FileProcessorRemote();
		fpDestination = new FileProcessorLocal();
	}
	else if (parser.GetMode() == WorkMode::Push)
	{
		fpSource = new FileProcessorLocal();
		fpDestination = new FileProcessorRemote();
	}
	else
	{
		MessagePrinter::PrintResult(Result::Unknown);
		FTPS_Assert(false);
		return -1;
	}

	Result res = Process({ &parser, &connection, &fileUtil }, fpSource, fpDestination);

	delete fpDestination;
	delete fpSource;

	if (res != Result::OK)
	{
		MessagePrinter::PrintResult(res);
		return -1;
	}
	else
	{
		return 0;
	}
}
