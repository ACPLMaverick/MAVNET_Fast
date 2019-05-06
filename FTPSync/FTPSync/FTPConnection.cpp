#include "FTPConnection.h"
#include "CommandParser.h"
#include "FileList.h"

#define ConnectionCheck()			\
{									\
	if(!m_ftpClient.IsConnected())	\
	{								\
		FTPS_Assert(false);			\
		return false;				\
	}								\
}

FTPConnection::FTPConnection(const CommandParser * parser)
{
	m_parser = parser;

	nsFTP::CLogonInfo logonInfo
	(
		m_parser->GetHostname().IpAddress,
		m_parser->GetHostname().Port,
		m_parser->GetLogin(),
		m_parser->GetPassword()
	);

	m_ftpClient.Login(logonInfo);
}

FTPConnection::~FTPConnection()
{
	if (m_ftpClient.IsConnected())
	{
		m_ftpClient.Logout();
	}
	m_parser = nullptr;
}

bool FTPConnection::PushDirectory()
{
	ConnectionCheck();

	std::string workingDir;
	if (m_ftpClient.GetWorkingDirectory(workingDir) != nsFTP::FTP_OK)
	{
		return false;
	}

	m_directoryStack.push(workingDir);

	return true;
}

bool FTPConnection::PopDirectory()
{
	ConnectionCheck();

	if (m_directoryStack.empty())
	{
		return false;
	}

	std::string topDir = m_directoryStack.top();
	m_directoryStack.pop();

	return ChangeDirectory(topDir);
}

bool FTPConnection::ChangeDirectory(const std::string& directory)
{
	ConnectionCheck();

	return m_ftpClient.ChangeWorkingDirectory(directory) == nsFTP::FTP_OK;
}

bool FTPConnection::RemoveFile(const std::string & path)
{
	ConnectionCheck();

	return m_ftpClient.Delete(path) == nsFTP::FTP_OK;
}

bool FTPConnection::IsDirectoryExist(const std::string & path)
{
	ConnectionCheck();

	return m_ftpClient.Status(path) == nsFTP::FTP_OK;
}

bool FTPConnection::GetFilesInDirectory(const std::string & directory, const std::vector<std::string>& filters, FileList & outFileList)
{
	ConnectionCheck();

	nsFTP::TFTPFileStatusShPtrVec list;
	if (!m_ftpClient.List(directory, list))
	{
		return false;
	}

	for (nsFTP::TFTPFileStatusShPtrVec::iterator it = list.begin(); it != list.end(); ++it)
	{
		bool bCanAdd = true;
		if (!filters.empty())
		{
			bCanAdd = false;
			for (const std::string& filter : filters)
			{
				if ((*it)->Name().find(filter) != std::string::npos)
				{
					bCanAdd = true;
					break;
				}
			}
		}

		if(bCanAdd)
		{
			File& file = outFileList.Append();
			file.Name = (*it)->Name();
			file.Status = FileStatus::Unknown;
		}
	}

	return true;
}

bool FTPConnection::PushFile(const std::string & source, const std::string & destination)
{
	ConnectionCheck();

	return m_ftpClient.UploadFile(source, destination);
}

bool FTPConnection::PullFile(const std::string & source, const std::string & destination)
{
	ConnectionCheck();

	return m_ftpClient.DownloadFile(source, destination);
}
