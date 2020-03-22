#pragma once

#include "Common.h"
#include "FilePath.h"

#include <rapidjson/document.h>

class Serializable;

using PropertyOffset = size_t;
using PropertyDatabase = std::vector<PropertyOffset>;

class PropertyBase
{
public:

	PropertyBase(const char* name);
	PropertyBase(const char* name, PropertyDatabase& database, PropertyOffset offset);

	virtual void Serialize(rapidjson::Document& doc, rapidjson::Value& root) = 0;
	virtual void Deserialize(rapidjson::Value& root) = 0;

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
	~Property();

	void Serialize(rapidjson::Document& doc, rapidjson::Value& root) override;
	void Deserialize(rapidjson::Value& root) override;

	Property& operator=(const InnerType& inner)
	{
		m_value = inner;
		return *this;
	}

	operator InnerType&()
	{
		return m_value;
	}

	operator const InnerType&() const
	{
		return m_value;
	}

	InnerType& Get() { return m_value; }
	const InnerType& Get() const { return m_value; }

protected:

	InnerType m_value;
};

class Serializable : public PropertyBase
{
public:

	Serializable();
	Serializable(const char* name);
	~Serializable();

	// Inherited via PropertyBase
	void Serialize(rapidjson::Document & doc, rapidjson::Value & root) override final;
	void Deserialize(rapidjson::Value& root) override final;

	bool LoadFromFile();
	bool SaveToFile();
	bool DeleteMyFile();

protected:

	const FilePath GetFilePath();
	virtual const FilePath GetFilePath_Internal() = 0;

	PropertyDatabase m_propertyDatabase;

#define LRT_PROPERTY(_outerType_, _type_, _name_, _defVal_)				\
protected:																\
	Property<_type_> m_##_name_{_defVal_, #_name_, m_propertyDatabase, offsetof(_outerType_, m_##_name_)};						\
public:																	\
	const _type_& Get_##_name_() const { return m_##_name_; }			\
	_type_& Get_##_name_() { return m_##_name_; }						\
	void Set_##_name_(const _type_& value) { m_##_name_ = value; }

private:

	bool LoadJSONFromFile(std::string& outJSON);
	bool SaveJSONToFile(const std::string& JSON);
	PropertyBase* GetPropertyFromOffset(PropertyOffset offset);
};