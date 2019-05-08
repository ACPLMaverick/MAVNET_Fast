#pragma once

#include "FTPSync.h"

#include "Error.h"
#include "AppState.h"

class FileList;
struct File;

class MessagePrinter
{
public:

	static void PrintResult(Result error)
	{
		PrintLine(((error == Result::OK || error == Result::OKPartial) ? AppTag : ErrorTag) + std::string(ErrorMessages[(size_t)error]));
	}

	static void PrintAppState(AppState state)
	{
		PrintLine(AppTag + std::string(StateMessages[(size_t)state]));
	}

	static void PrintFileList(const FileList& fileList);
	static void PrintFile(const File& file);

	static void PrintLine(const std::string& line)
	{
		std::cout << line << std::endl;
	}

private:

	static const char* ErrorMessages[];
	static const char* StateMessages[];
	static const char* FileStatusIcons[];
	static const char* AppTag;
	static const char* ErrorTag;
};