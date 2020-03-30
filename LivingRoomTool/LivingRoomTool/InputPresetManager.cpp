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

void InputPresetManager::DuplicatePreset(size_t a_index, const std::string& newName)
{
	LRT_InputPresetManagerCheckIndex(a_index);
	LRT_Todo();
}

void InputPresetManager::RenamePreset(size_t a_index, const std::string& newName)
{
	LRT_InputPresetManagerCheckIndex(a_index);

	// When renaming a preset, remember to save it, create a new one with a new name, 
	// and remove the old one. This should be the safest approach.
	// I should also display a prompt signalling that the preset will be saved when renamed.
	LRT_Todo();
}

void InputPresetManager::RemovePreset(size_t a_index)
{
	LRT_InputPresetManagerCheckIndex(a_index);
	const bool success = m_presets[a_index].DeleteMyFile();
	LRT_Assert(success);
}

void InputPresetManager::SavePreset(size_t a_index)
{
	LRT_InputPresetManagerCheckIndex(a_index);
	const bool success = m_presets[a_index].SaveToFile();
	LRT_Assert(success);
}

void InputPresetManager::RestorePreset(size_t a_index)
{
	LRT_InputPresetManagerCheckIndex(a_index);
	const bool success = m_presets[a_index].LoadFromFile();
	LRT_Assert(success);
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
