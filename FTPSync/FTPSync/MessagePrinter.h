#pragma once

#include "FTPSync.h"

#include "Error.h"

class MessagePrinter
{
public:

	static void PrintResult(Result error)
	{
		PrintLine("ERROR: " + std::string(ErrorMessages[(size_t)error]));
	}

private:

	static void PrintLine(const std::string& line)
	{
		std::cout << line << std::endl;
	}

	static const char* ErrorMessages[];
};