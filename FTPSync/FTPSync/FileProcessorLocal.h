#pragma once

#include "FileProcessor.h"

class FileProcessorLocal : public FileProcessor
{
protected:

	// Inherited via FileProcessor
	virtual void GetPathFromParser(const CommandParser* parser, std::string& outPath) override;
	virtual FileInterface* GetLocalFileInterface() override;
	virtual AppState GetThisAppState() override;
	virtual AppState GetSyncAppState() override;
	virtual bool PerformFileAdd(const File& file) override;
	virtual bool PerformFileRemove(const File& file) override;
};