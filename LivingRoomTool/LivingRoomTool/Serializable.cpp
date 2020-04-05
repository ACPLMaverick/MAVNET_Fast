#include "Serializable.h"

#include "GamepadConfig.h"
#include "InputAction.h"
#include "InputBinding.h"
#include "GamepadState.h"

#include <Windows.h>

#include <magic_enum/magic_enum.hpp>

#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

PropertyBase::PropertyBase(const char * a_name)
	: m_name(a_name)
{
}

PropertyBase::PropertyBase(const char* a_name, PropertyDatabase & a_database, PropertyOffset a_offset)
	: m_name(a_name)
{
	a_database.push_back(a_offset);
}

#define LRT_SpecializePropertyFuncsConvert(_type_, _rjType_, _convertTo_, _convertFrom_)\
template<> Property<_type_>::~Property<_type_>() {}							\
template<>																	\
void Property<_type_>::Serialize(rapidjson::Document& a_doc, rapidjson::Value& a_root)\
{																			\
	auto nameRef = rapidjson::StringRef(m_name.c_str(), m_name.size());		\
	a_root.AddMember(nameRef, _convertTo_(m_value), a_doc.GetAllocator());	\
}																			\
																			\
template<>																	\
void Property<_type_>::Deserialize(rapidjson::Value& a_value)				\
{																			\
	m_value = _convertFrom_(a_value[m_name.c_str()].Get##_rjType_());		\
}																			\
template<>																	\
void Property<std::vector<_type_>>::Serialize(rapidjson::Document& a_doc, rapidjson::Value& a_root)	\
{																			\
	rapidjson::Value arrayOfValues(rapidjson::kArrayType);					\
	for(_type_& obj : m_value)												\
	{																		\
		rapidjson::Value value(_convertTo_(obj));							\
		arrayOfValues.PushBack(value, a_doc.GetAllocator());				\
	}																		\
	auto nameRef = rapidjson::StringRef(m_name.c_str(), m_name.size());		\
	a_root.AddMember(nameRef, arrayOfValues, a_doc.GetAllocator());			\
}																			\
																			\
template<>																	\
void Property<std::vector<_type_>>::Deserialize(rapidjson::Value& a_value)	\
{																			\
	rapidjson::Value& arrayOfValues = a_value[m_name.c_str()];				\
	LRT_Assert(arrayOfValues.IsArray());									\
	for(auto it = arrayOfValues.Begin(); it != arrayOfValues.End(); ++it)	\
	{																		\
		m_value.push_back(_convertFrom_(it->Get##_rjType_()));				\
	}																		\
}
#define LRT_SpecializePropertyFuncsCast(_type_, _rjType_, _castType_)	\
	LRT_SpecializePropertyFuncsConvert(_type_, _rjType_, static_cast<_castType_>, static_cast<_type_>)
#define LRT_SpecializePropertyFuncs(_type_, _rjType_)	\
	LRT_SpecializePropertyFuncsCast(_type_, _rjType_, _type_)

#define LRT_SpecializePropertyFuncsEnumConvert(_type_, _convertToFunc_, _convertFromFunc_)	\
template<> Property<_type_>::~Property<_type_>() {}							\
template<> Property<std::vector<_type_>>::~Property<std::vector<_type_>>() {}\
template<>																	\
void Property<_type_>::Serialize(rapidjson::Document& a_doc, rapidjson::Value& a_root)\
{																			\
	auto nameRef = rapidjson::StringRef(m_name.c_str(), m_name.size());		\
	auto enumToText = _convertToFunc_(m_value);								\
	LRT_Assert(enumToText.data() != nullptr);								\
	auto valueRef = rapidjson::StringRef(enumToText.data());				\
	a_root.AddMember(nameRef, valueRef, a_doc.GetAllocator());				\
}																			\
																			\
template<>																	\
void Property<_type_>::Deserialize(rapidjson::Value& a_value)				\
{																			\
	auto optional = _convertFromFunc_(a_value[m_name.c_str()].GetString());	\
	if(optional.has_value())												\
	{																		\
		m_value = optional.value();											\
	}																		\
	else																	\
	{																		\
		LRT_Fail();															\
		m_value = static_cast<_type_>(0);									\
	}																		\
}																			\
template<>																	\
void Property<std::vector<_type_>>::Serialize(rapidjson::Document& a_doc, rapidjson::Value& a_root)	\
{																			\
	rapidjson::Value arrayOfValues(rapidjson::kArrayType);					\
	for(_type_ obj : m_value)												\
	{																		\
		auto enumToText = _convertToFunc_(obj);								\
		LRT_Assert(enumToText.data() != nullptr);							\
		auto valueRef = rapidjson::StringRef(enumToText.data(), enumToText.size());\
		rapidjson::Value value(valueRef);									\
		arrayOfValues.PushBack(value, a_doc.GetAllocator());				\
	}																		\
	auto nameRef = rapidjson::StringRef(m_name.c_str(), m_name.size());		\
	a_root.AddMember(nameRef, arrayOfValues, a_doc.GetAllocator());			\
}																			\
																			\
template<>																	\
void Property<std::vector<_type_>>::Deserialize(rapidjson::Value& a_value)	\
{																			\
	rapidjson::Value& arrayOfValues = a_value[m_name.c_str()];				\
	LRT_Assert(arrayOfValues.IsArray());									\
	for(auto it = arrayOfValues.Begin(); it != arrayOfValues.End(); ++it)	\
	{																		\
		_type_ tempVal;														\
		auto optional = _convertFromFunc_(it->GetString());					\
		if(optional.has_value())											\
		{																	\
			tempVal = optional.value();										\
		}																	\
		else																\
		{																	\
			LRT_Fail();														\
			tempVal = static_cast<_type_>(0);								\
		}																	\
		m_value.push_back(tempVal);											\
	}																		\
}

#define LRT_SpecializePropertyFuncsEnum(_type_) LRT_SpecializePropertyFuncsEnumConvert(_type_, magic_enum::enum_name, magic_enum::enum_cast<_type_>)

#define LRT_SpecializePropertyFuncsSerializable(_type_)									\
template<>																				\
Property<_type_*>::~Property<_type_*>()													\
{																						\
	if (m_value != nullptr)																\
	{																					\
		delete m_value;																	\
		m_value = nullptr;																\
	}																					\
}																						\
																						\
template<>																				\
Property<std::vector<_type_*>>::~Property<std::vector<_type_*>>()						\
{																						\
	for(_type_* obj : m_value)															\
	{																					\
		if (obj != nullptr)																\
		{																				\
			delete obj;																	\
		}																				\
	}																					\
	m_value.clear();																	\
}																						\
																						\
template<>																				\
void Property<_type_*>::Serialize(rapidjson::Document& a_doc, rapidjson::Value& a_root)	\
{																						\
	rapidjson::Value value(rapidjson::kObjectType);										\
	if (m_value != nullptr)																\
	{																					\
		m_value->Serialize(a_doc, value);												\
	}																					\
	else																				\
	{																					\
		value.SetNull();																\
	}																					\
	auto nameRef = rapidjson::StringRef(m_name.c_str(), m_name.size());					\
	a_root.AddMember(nameRef, value, a_doc.GetAllocator());								\
}																						\
																						\
template<>																				\
void Property<_type_*>::Deserialize(rapidjson::Value& a_value)							\
{																						\
	if (m_value != nullptr)																\
	{																					\
		rapidjson::Value& value = a_value[m_name.c_str()];								\
		m_value->Deserialize(value);													\
	}																					\
}																						\
																						\
template<>																				\
void Property<std::vector<_type_*>>::Serialize(rapidjson::Document& a_doc, rapidjson::Value& a_root)\
{																						\
	rapidjson::Value arrayOfValues(rapidjson::kArrayType);								\
	for(_type_* obj : m_value)															\
	{																					\
		rapidjson::Value value(rapidjson::kObjectType);									\
		if (obj != nullptr)																\
		{																				\
			obj->Serialize(a_doc, value);												\
		}																				\
		else																			\
		{																				\
			value.SetNull();															\
		}																				\
		arrayOfValues.PushBack(value, a_doc.GetAllocator());							\
	}																					\
	auto nameRef = rapidjson::StringRef(m_name.c_str(), m_name.size());					\
	a_root.AddMember(nameRef, arrayOfValues, a_doc.GetAllocator());						\
}																						\
																						\
template<>																				\
void Property<std::vector<_type_*>>::Deserialize(rapidjson::Value& a_value)				\
{																						\
	rapidjson::Value& arrayOfValues = a_value[m_name.c_str()];							\
	LRT_Assert(arrayOfValues.IsArray());												\
	m_value.resize(arrayOfValues.Size());												\
	size_t i = 0;																		\
	for (auto it = arrayOfValues.Begin(); it != arrayOfValues.End(); ++it, ++i)			\
	{																					\
		m_value[i] = new _type_();														\
		m_value[i]->Deserialize(*it);													\
	}																					\
}

LRT_SpecializePropertyFuncs(float, Float)
LRT_SpecializePropertyFuncsEnum(GamepadConfig::InstrumentationMode)
LRT_SpecializePropertyFuncsEnumConvert(GamepadButtons, GamepadButtonsConvert::ToString, GamepadButtonsConvert::FromString)
LRT_SpecializePropertyFuncsEnum(InputActionKey)
LRT_SpecializePropertyFuncsSerializable(InputBinding)

Serializable::Serializable()
	: PropertyBase(__func__)
{
}

Serializable::Serializable(const char* a_name)
	: PropertyBase(a_name)
{
}

Serializable::~Serializable()
{
}

void Serializable::Serialize(rapidjson::Document & a_doc, rapidjson::Value & a_root)
{
	for (PropertyOffset propOffset : m_propertyDatabase)
	{
		PropertyBase* prop = GetPropertyFromOffset(propOffset);
		prop->Serialize(a_doc, a_root);
	}
}

void Serializable::Deserialize(rapidjson::Value& a_value)
{
	for (PropertyOffset propOffset : m_propertyDatabase)
	{
		PropertyBase* prop = GetPropertyFromOffset(propOffset);
		prop->Deserialize(a_value);
	}
}

bool Serializable::LoadFromFile()
{
	std::string json;
	if (LoadJSONFromFile(json) == false)
	{
		return false;
	}

	rapidjson::Document document;
	document.Parse(json.c_str());
	if (document.IsObject() == false)
	{
		LRT_Fail();
		return false;
	}

	Deserialize(document);

	return true;
}

bool Serializable::SaveToFile()
{
	rapidjson::Document document;	// New empty document.
	rapidjson::Value& value = document.SetObject();	// Root object.

	Serialize(document, value);

	typedef rapidjson::GenericStringBuffer<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<>> StringBuffer;
	rapidjson::StringBuffer buf;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
	writer.SetMaxDecimalPlaces(4);
	document.Accept(writer);
	std::string json(buf.GetString(), buf.GetSize());

	if (SaveJSONToFile(json) == false)
	{
		return false;
	}

	return true;
}

bool Serializable::DeleteMyFile()
{
	const FilePath& filePath = GetFilePath();
	return DeleteFileW(filePath.GetFullFile().c_str()) != FALSE;
}

const FilePath Serializable::GetFilePath()
{
	FilePath filePath;

	const std::wstring& baseDir = FilePath::GetBaseDirectory();
	if (baseDir.empty() == false)
	{
		FilePath filePathInternal = GetFilePath_Internal();
		filePath = FilePath(baseDir + (filePathInternal.GetDirectory().empty() ? L"" : L"\\") + filePathInternal.GetDirectory(), filePathInternal.GetFile() + L".json");
	}

	return filePath;
}

bool Serializable::LoadJSONFromFile(std::string& a_outJSON)
{
	const FilePath& filePath = GetFilePath();

	HANDLE file = CreateFileW(
		filePath.GetFullFile().c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (file == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	LARGE_INTEGER byteNum{};
	if (GetFileSizeEx(file, &byteNum) == FALSE)
	{
		LRT_Verify(CloseHandle(file));
		return false;
	}

	if (byteNum.QuadPart == 0)
	{
		LRT_Verify(CloseHandle(file));
		return false;
	}

	a_outJSON.resize(byteNum.QuadPart, 0);
	DWORD bytesRead{ 0 };
	if (ReadFile(file, const_cast<char*>(a_outJSON.data()), byteNum.QuadPart, &bytesRead, NULL) == FALSE)
	{
		LRT_Verify(CloseHandle(file));
		return false;
	}

	const bool bSuccess = bytesRead == byteNum.QuadPart;
	LRT_Assert(bSuccess);

	LRT_Verify(CloseHandle(file));

	return bSuccess;
}

bool Serializable::SaveJSONToFile(const std::string& a_JSON)
{
	const FilePath& filePath = GetFilePath();
	filePath.MakeSureDirectoryExists();

	HANDLE file = CreateFileW(
		filePath.GetFullFile().c_str(),
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (file == INVALID_HANDLE_VALUE)
	{
		LRT_PrintLastError();
		return false;
	}

	DWORD bytesWritten{ 0 };
	if (WriteFile(file, a_JSON.c_str(), a_JSON.size(), &bytesWritten, NULL) == FALSE)
	{
		LRT_PrintLastError();
		LRT_Verify(CloseHandle(file));
		return false;
	}

	const bool bSuccess = bytesWritten == a_JSON.size();
	LRT_Assert(bSuccess);

	LRT_Verify(CloseHandle(file));

	return bSuccess;
}

PropertyBase* Serializable::GetPropertyFromOffset(PropertyOffset a_offset)
{
	return reinterpret_cast<PropertyBase*>(reinterpret_cast<uint8_t*>(this) + a_offset);
}
