#include "MessagePrinter.h"
#include "FileList.h"

const char* MessagePrinter::ErrorMessages[] = 
{
	"Success.",
	"Partial success."
	"Local directory does not exist.",
	"Remote directory does not exist.",
	"No connection with FTP server.",
	"Transfer error during file sync.",
	"Invalid work mode specified.",
	"Feature not implemented."
	"Unknown error."
};

const char* MessagePrinter::StateMessages[] = 
{
	"Connecting to remote server.",
	"Gathering local filelist.",
	"Gathering remote filelist.",
	"Performing file diff.",
	"All files are in sync. Nothing to do.",
	"Performing file Push (LOCAL => REMOTE).",
	"Performing file Pull (REMOTE => LOCAL)."
};

const char* MessagePrinter::FileStatusIcons[] =
{
	"[?] ",
	"[ ] ",
	"[+] ",
	"[-] "
};

const char* MessagePrinter::AppTag = "[FTP Sync] ";
const char* MessagePrinter::ErrorTag = "!!! ERROR !!! ";

void MessagePrinter::PrintFileList(const FileList & fileList)
{
	for (size_t i = 0; i < fileList.GetFileNum(); ++i)
	{
		const File& file = fileList[i];
		if (file.Status != FileStatus::OK)
		{
			PrintFile(file);
		}
	}
}

void MessagePrinter::PrintFile(const File & file)
{
	const std::string fileLine = std::string(AppTag) + FileStatusIcons[(size_t)file.Status] + file.Name;
	PrintLine(fileLine);
}
