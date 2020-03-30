#pragma once

#include "Common.h"
#include "InputPreset.h"

class InputPresetManager
{
public:

	void LoadPresets();

	size_t GetPresetNum() const { return m_presets.size(); }
	InputPreset& GetPreset(size_t index);
	const InputPreset& GetPreset(size_t index) const;

	void DuplicatePreset(size_t index, const std::string& newName);
	void RenamePreset(size_t index, const std::string& newName);
	void RemovePreset(size_t index);
	void SavePreset(size_t index);
	void RestorePreset(size_t index);

	static const std::wstring& GetDirectory();

private:

	static const std::wstring& GetFullDirectory();
	static void GetExistingPresetNames(std::vector<std::wstring>& outNames);

	std::vector<InputPreset> m_presets;
};

