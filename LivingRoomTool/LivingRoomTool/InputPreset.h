#pragma once

#include "Common.h"
#include "Serializable.h"
#include "InputBinding.h"

// It contains collection of 
// multiple GamepadButton assignations to multiple Keyboard/Mouse keys.
// It also has ways of getting saved and loaded from file.
class InputPreset : public Serializable
{
public:

	InputPreset(const char* name);
	InputPreset(InputPreset&& move);
	virtual ~InputPreset();

	void GenerateActions(const GamepadState& gamepadState, const GamepadConfig& gamepadConfig, std::vector<InputAction>& outActions) const;
	void Rename(const std::string& newName);

	static InputPreset CreateDefault();

protected:

	// Inherited via Serializable
	virtual const FilePath GetFilePath_Internal() override;

	LRT_PROPERTY(InputPreset, std::vector<InputBinding*>, bindings, {});
};
