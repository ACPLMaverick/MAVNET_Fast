#pragma once

#include "FTPSync.h"

class MessagePrinter
{
public:

	enum class Error : uint8_t
	{
		InvalidWorkMode,
		Unknown
	};

	static void PrintError(Error error)
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