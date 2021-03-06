#include "InputPresetManager.h"

#include "FilePath.h"

#include <Windows.h>

#define LRT_InputPresetManagerCheckIndex(index)	\
if(index >= m_presets.size()) { LRT_Fail(); }

void InputPresetManager::LoadPresets()
{
	m_presets.clear();

	std::vector<std::wstring> existingPresetNames;
	GetExistingPresetNames(existingPresetNames);

	if (existingPresetNames.size() == 0)
	{
		// No presets found. Create a default one and save it.
		m_presets.emplace_back(InputPreset::CreateDefault());
		SavePreset(0);
	}
	else
	{
		// For each preset name, load it into our database.
		for (const std::wstring& wname : existingPresetNames)
		{
			const std::string name(wname.begin(), wname.end());
			m_presets.emplace_back(name.c_str());// This creates object directly inside vec.
			LRT_Verify(m_presets.back().LoadFromFile());
		}
	}
}

InputPreset& InputPresetManager::GetPreset(size_t a_index)
{
	LRT_InputPresetManagerCheckIndex(a_index);
	return m_presets[a_index];
}

const InputPreset& InputPresetManager::GetPreset(size_t a_index) const
{
	LRT_InputPresetManagerCheckIndex(a_index);
	return m_presets[a_index];
}

void InputPresetManager::DuplicatePreset(size_t a_index, const std::string& a_newName)
{
	LRT_InputPresetManagerCheckIndex(a_index);
	m_presets.push_back(std::move(InputPreset::DeepCopy(m_presets[a_index], a_newName.c_str())));
	LRT_Verify(m_presets.back().SaveToFile());
}

void InputPresetManager::RenamePreset(size_t a_index, const std::string& a_newName)
{
	LRT_InputPresetManagerCheckIndex(a_index);
	if (m_presets[a_index].GetName() == a_newName)
	{
		return;
	}

	// We can make a shallow copy here.
	InputPreset presetWithNewName(std::move(m_presets[a_index]), a_newName.c_str());
	LRT_Verify(presetWithNewName.SaveToFile());
	LRT_Verify(m_presets[a_index].DeleteMyFile());
	m_presets[a_index] = std::move(presetWithNewName);
}

void InputPresetManager::RemovePreset(size_t a_index)
{
	LRT_InputPresetManagerCheckIndex(a_index);
	const bool success = m_presets[a_index].DeleteMyFile();
	LRT_Assert(success);

	if (success)
	{
		m_presets.erase(m_presets.begin() + a_index);
	}
}

void InputPresetManager::SavePreset(size_t a_index)
{
	LRT_InputPresetManagerCheckIndex(a_index);
	LRT_Verify(m_presets[a_index].SaveToFile());
}

void InputPresetManager::RestorePreset(size_t a_index)
{
	LRT_InputPresetManagerCheckIndex(a_index);
	LRT_Verify(m_presets[a_index].LoadFromFile());
}

bool InputPresetManager::FindPresetByName(const std::string & a_name, size_t* a_outIndex) const
{
	const size_t numPresets = GetPresetNum();

	for (size_t i = 0; i < numPresets; ++i)
	{
		if (m_presets[i].GetName() == a_name)
		{
			if (a_outIndex != nullptr)
			{
				*a_outIndex = i;
			}
			return true;
		}
	}

	return false;
}

const std::wstring & InputPresetManager::GetDirectory()
{
	static const std::wstring directory = L"presets";
	return directory;
}

const std::wstring & InputPresetManager::GetFullDirectory()
{
	static const std::wstring directory = FilePath::GetBaseDirectory() + L"\\" + GetDirectory();
	return directory;
}

void InputPresetManager::GetExistingPresetNames(std::vector<std::wstring>& outNames)
{
	std::wstring dir = GetFullDirectory();
	FilePath::MakeSureDirectoryExists(dir);

	dir += L"\\*.json";

	WIN32_FIND_DATA data{};
	HANDLE file = FindFirstFile(dir.c_str(), &data);
	if (file != INVALID_HANDLE_VALUE)
	{
		do
		{
			std::wstring fileName = data.cFileName;
			
			// Strip the extension.
			fileName = fileName.substr(0, fileName.find('.'));

			outNames.push_back(fileName);
		} while (FindNextFile(file, &data));
		FindClose(file);
	}
}
