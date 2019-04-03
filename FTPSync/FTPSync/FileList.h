#pragma once

#include "FTPSync.h"

class FileList
{
public:

	typedef std::vector<std::string> Files;

	Files& GetFiles() { return m_files; }

private:

	Files m_files;
};