#pragma once

#include "FileProcessor.h"

class FileProcessorRemote : public FileProcessor
{
protected:

	// Inherited via FileProcessor
	virtual Result Initialize_Internal() override;
	virtual Result Cleanup_Internal() override;
	virtual Result PerformSync() override;
};