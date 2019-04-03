// FTPSync.cpp : Defines the entry point for the application.
//

#include "FTPSync.h"

#include "CommandParser.h"
#include "MessagePrinter.h"

#include "FileProcessorLocal.h"
#include "FileProcessorRemote.h"

using namespace std;

int main(int argc, char* argv[])
{
	CommandParser parser(argc, argv);

	FileProcessor* fpSource(nullptr);
	FileProcessor* fpDestination(nullptr);

	if (parser.GetMode() == WorkMode::Unknown)
	{
		MessagePrinter::PrintError(MessagePrinter::Error::InvalidWorkMode);
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
		MessagePrinter::PrintError(MessagePrinter::Error::Unknown);
		FTPS_Assert(false);
		return -1;
	}

	delete fpSource;
	delete fpDestination;

	return 0;
}
