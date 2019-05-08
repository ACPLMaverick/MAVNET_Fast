#pragma once

#include "FTPSync.h"

enum class Result : uint8_t
{
	OK,
	OKPartial,
	NoLocalDir,
	NoRemoteDir,
	NoConnection,
	Transfer,
	InvalidWorkMode,
	NotImplemented,
	Unknown
};