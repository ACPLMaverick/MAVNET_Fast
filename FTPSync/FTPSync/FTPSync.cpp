// FTPSync.cpp : Defines the entry point for the application.
//

#include "FTPSync.h"

#include "CommandParser.h"
#include "MessagePrinter.h"

#include "FileProcessorLocal.h"
#include "FileProcessorRemote.h"

#include "Error.h"

Result Process(const CommandParser* parser, FileProcessor* fpSource, FileProcessor* fpDestination)
{
	assert(fpSource);
	assert(fpDestination);
	
	Result res = fpSource->Initialize(parser);
	if (res != Result::OK)
	{
		return res;
	}
	fpDestination->Initialize(parser);
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

	Result res = Process(&parser, fpSource, fpDestination);

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
