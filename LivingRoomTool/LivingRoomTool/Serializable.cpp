#include "Serializable.h"

#include "GamepadConfig.h"

#include <Windows.h>
#include <ShlObj_core.h>

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

#define LRT_SpecializePropertyFuncsCast(_type_, _rjType_, _castType_)		\
template<> Property<_type_>::~Property<_type_>() {}							\
template<>																	\
void Property<_type_>::Serialize(rapidjson::Document& a_doc, rapidjson::Value& a_root)\
{																			\
	auto nameRef = rapidjson::StringRef(m_name.c_str(), m_name.size());		\
	a_root.AddMember(nameRef, static_cast<_castType_>(m_value), a_doc.GetAllocator());\
}																			\
																			\
template<>																	\
void Property<_type_>::Deserialize(rapidjson::Value& a_value)				\
{																			\
	m_value = static_cast<_type_>(a_value[m_name.c_str()].Get##_rjType_());	\
}																			\
template<>																	\
void Property<std::vector<_type_>>::Serialize(rapidjson::Document& a_doc, rapidjson::Value& a_root)	\
{																			\
	rapidjson::Value arrayOfValues(rapidjson::kArrayType);					\
	for(_type_& obj : m_value)												\
	{																		\
		rapidjson::Value value(static_cast<_castType_>(obj));				\
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
		m_value.push_back(static_cast<_type_>(it->Get##_rjType_()));		\
	}																		\
}
#define LRT_SpecializePropertyFuncs(_type_, _rjType_) LRT_SpecializePropertyFuncsCast(_type_, _rjType_, _type_)

LRT_SpecializePropertyFuncs(float, Float)
LRT_SpecializePropertyFuncsCast(GamepadConfig::InstrumentationMode, Uint, unsigned int)

template<> Property<Serializable*>::~Property<Serializable*>()
{
	if (m_value != nullptr)
	{
		delete m_value;
		m_value = nullptr;
	}
}

template<> void Property<Serializable*>::Serialize(rapidjson::Document& a_doc, rapidjson::Value& a_root)
{
	rapidjson::Value value(rapidjson::kObjectType);
	if (m_value != nullptr)
	{
		m_value->Serialize(a_doc, value);
	}
	else
	{
		value.SetNull();
	}
	auto nameRef = rapidjson::StringRef(m_name.c_str(), m_name.size());
	a_root.AddMember(nameRef, value, a_doc.GetAllocator());
}

template<> void Property<Serializable*>::Deserialize(rapidjson::Value& a_value)
{
	if (m_value != nullptr)
	{
		rapidjson::Value& value = a_value[m_name.c_str()];
		m_value->Deserialize(value);
	}
}

template<> void Property<std::vector<Serializable*>>::Serialize(rapidjson::Document& a_doc, rapidjson::Value& a_root)
{
	rapidjson::Value arrayOfValues(rapidjson::kArrayType);
	for(Serializable* obj : m_value)
	{
		rapidjson::Value value(rapidjson::kObjectType);
		if (obj != nullptr)
		{
			obj->Deserialize(value);
		}
		else
		{
			value.SetNull();
		}
		arrayOfValues.PushBack(value, a_doc.GetAllocator());
	}
	auto nameRef = rapidjson::StringRef(m_name.c_str(), m_name.size());
	a_root.AddMember(nameRef, arrayOfValues, a_doc.GetAllocator());
}

template<> void Property<std::vector<Serializable*>>::Deserialize(rapidjson::Value& a_value)
{
	rapidjson::Value& arrayOfValues = a_value[m_name.c_str()];
	LRT_Assert(arrayOfValues.IsArray());
	LRT_Assert(arrayOfValues.Size() == m_value.size()); // Value objects must be created.
	size_t i = 0;
	for (auto it = arrayOfValues.Begin(); it != arrayOfValues.End(); ++it, ++i)
	{
		m_value[i]->Deserialize(*it);
	}
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

const Serializable::FilePath& Serializable::GetFilePath()
{
	static FilePath filePath;

	if (filePath.IsEmpty())
	{
		wchar_t* pathPtr(nullptr);
		LRT_CheckHR(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, NULL, &pathPtr));
		if (pathPtr != nullptr)
		{
			FilePath filePathInternal = GetFilePath_Internal();
			filePath = FilePath(std::wstring(pathPtr) + L"\\LivingRoomTool" + (filePathInternal.GetDirectory().empty() ? L"" : L"\\") + filePathInternal.GetDirectory(), filePathInternal.GetFile() + L".json");
			CoTaskMemFree(pathPtr);
		}
	}

	return filePath;
}

bool Serializable::LoadJSONFromFile(std::string& a_outJSON)
{
	HANDLE file = CreateFileW(
		GetFilePath().GetFullFile().c_str(),
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
	MakeSureDirectoryExists(filePath.GetDirectory());

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

inline void Serializable::MakeSureDirectoryExists(const std::wstring& a_directory)
{
	// Do not proceed if there is only drive leter left.
	if (a_directory.length() <= 3)
	{
		return;
	}

	// Simply try to create that directory without caring about errors.
	BOOL ret = CreateDirectoryW(a_directory.c_str(), NULL);
	if (ret == FALSE)
	{
		if (GetLastError() == ERROR_PATH_NOT_FOUND)
		{
			// Go backwards until we find a valid directory.
			size_t splitPos = a_directory.find_last_of('\\');
			if (splitPos != std::string::npos)
			{
				std::wstring cutDirectory = a_directory.substr(0, splitPos);
				MakeSureDirectoryExists(cutDirectory);
			}
			// Try to create it again.
			ret = CreateDirectoryW(a_directory.c_str(), NULL);
			LRT_Assert(ret == TRUE);	// We should not fail this time.
		}
	}
}

PropertyBase* Serializable::GetPropertyFromOffset(PropertyOffset a_offset)
{
	return reinterpret_cast<PropertyBase*>(reinterpret_cast<uint8_t*>(this) + a_offset);
}
