#pragma once

#include "FTPSync.h"

enum class FileStatus : uint8_t
{
	Unknown,
	OK,
	ToAdd,
	ToRemove
};

struct File
{
	std::string Name;
	FileStatus Status;

	bool operator==(const File& other)
	{
		return Name.compare(other.Name) == 0;
	}
};

typedef std::vector<File> Files;

class FileList
{
public:

	inline size_t GetFileNum() const { return m_files.size(); }
	inline bool IsEmpty() const { return m_files.empty(); }

	inline File& operator[](size_t index) { return m_files[index]; }
	inline const File& operator[](size_t index) const { return m_files[index]; }

	File& Append() { m_files.emplace_back(File()); return m_files.back(); }
	void Remove(const File& file)
	{
		Files::iterator it = std::find(m_files.begin(), m_files.end(), file);
		assert(it != m_files.end());
		if (it != m_files.end())
		{
			m_files.erase(it);
		}
	}
	void Remove(size_t index)
	{
		assert(index < GetFileNum());
		m_files.erase(m_files.begin() + index);
	}

	void Clear()
	{
		m_files.clear();
	}

private:

	Files m_files;
};