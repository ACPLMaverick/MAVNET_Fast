#pragma once

#include "Common.h"

#include <rapidjson/document.h>

class Serializable;

using PropertyOffset = size_t;
using PropertyDatabase = std::vector<PropertyOffset>;

class PropertyBase
{
public:

	PropertyBase(const char* name, PropertyDatabase& database, PropertyOffset offset);

	virtual void Serialize(rapidjson::Document& doc, rapidjson::Value& root) = 0;
	virtual void Deserialize(rapidjson::Document& doc) = 0;

	const std::string& GetName() const { return m_name; }

protected:

	std::string m_name;
};

template <typename InnerType> class Property : public PropertyBase
{
public:

	Property(InnerType a_defaultVal, const char* a_name, PropertyDatabase& a_database, PropertyOffset a_offset)
		: PropertyBase(a_name, a_database, a_offset)
		, m_value(a_defaultVal)
	{
	}

	void Serialize(rapidjson::Document& doc, rapidjson::Value& root) override;
	void Deserialize(rapidjson::Document& doc) override;

	Property& operator=(const InnerType& inner)
	{
		m_value = inner;
		return *this;
	}

	operator InnerType() const
	{
		return m_value;
	}

protected:

	InnerType m_value;
};


class Serializable
{
public:

	bool LoadFromFile();
	bool SaveToFile();

protected:

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

		const std::wstring& GetDirectory() const { return m_directory; }
		const std::wstring& GetFile() const { return m_file; }
		const std::wstring& GetFullFile() const { return m_fullFile; }
		bool IsEmpty() const { return m_directory.empty() || m_file.empty(); }

	private:
		std::wstring m_directory;
		std::wstring m_file;
		std::wstring m_fullFile;
	};

	const FilePath& GetFilePath();
	virtual const FilePath GetFilePath_Internal() = 0;

	PropertyDatabase m_propertyDatabase;

#define LRT_PROPERTY(_outerType_, _type_, _name_, _defVal_)				\
protected:																\
	Property<_type_> m_##_name_{_defVal_, #_name_, m_propertyDatabase, offsetof(_outerType_, m_##_name_)};						\
public:																	\
	const _type_& Get_##_name_() const { return m_##_name_; }

private:

	bool LoadJSONFromFile(std::string& outJSON);
	bool SaveJSONToFile(const std::string& JSON);
	inline void MakeSureDirectoryExists(const std::wstring& a_directory);
	PropertyBase* GetPropertyFromOffset(PropertyOffset offset);
};