#pragma once

#include "FileProcessor.h"

class FileProcessorLocal : public FileProcessor
{
protected:

	// Inherited via FileProcessor
	virtual FileInterface* GetLocalFileInterface() override;
	virtual Result PerformSync() override;
};