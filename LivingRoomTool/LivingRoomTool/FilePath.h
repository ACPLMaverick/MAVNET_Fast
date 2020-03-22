#pragma once

#include "Common.h"

class FilePath
{
public:

	FilePath()
		: m_directory()
		, m_file()
	{
	}

	FilePath(const std::wstring& directory, const std::wstring& file)
		: m_directory(directory)
		, m_file(file)
		, m_fullFile(directory + L"\\" + file)
	{
	}

	FilePath(const FilePath&& other)
		: m_directory(other.m_directory)
		, m_file(other.m_file)
		, m_fullFile(m_directory + L"\\" + m_file)
	{
	}

	FilePath& operator=(const FilePath&& other)
	{
		m_directory = other.m_directory;
		m_file = other.m_file;
		m_fullFile = (m_directory + L"\\" + m_file);
		return *this;
	}

	static const std::wstring& GetBaseDirectory();
	static void MakeSureDirectoryExists(const std::wstring& directory);

	void MakeSureDirectoryExists() const { MakeSureDirectoryExists(GetDirectory()); }
	const std::wstring& GetDirectory() const { return m_directory; }
	const std::wstring& GetFile() const { return m_file; }
	const std::wstring& GetFullFile() const { return m_fullFile; }
	bool IsEmpty() const { return m_directory.empty() || m_file.empty(); }

private:
	std::wstring m_directory;
	std::wstring m_file;
	std::wstring m_fullFile;
};
