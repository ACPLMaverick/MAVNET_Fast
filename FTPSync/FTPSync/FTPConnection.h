#pragma once

#include "FileInterface.h"
#include "FtpClient/FTPClient.h"

class CommandParser;

class FTPConnection : public FileInterface
{
public:

	FTPConnection(const CommandParser* parser);
	~FTPConnection();

	virtual bool PushDirectory() override;
	virtual bool PopDirectory() override;
	virtual bool ChangeDirectory(const std::string& directory) override;

	virtual bool RemoveFile(const std::string& path) override;
	virtual bool IsDirectoryExist(const std::string& path) override;
	virtual bool GetFilesInDirectory(const std::string& directory, const std::vector<std::string>& filters, FileList& outFileList) override;

	bool PushFile(const std::string& source, const std::string& destination);
	bool PullFile(const std::string& source, const std::string& destination);

private:

	nsFTP::CFTPClient m_ftpClient;
	const CommandParser* m_parser = nullptr;
};