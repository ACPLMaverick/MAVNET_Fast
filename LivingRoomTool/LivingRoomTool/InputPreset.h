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

	InputPreset(const char* name);										// Empty input preset with this name.
	InputPreset(const InputPreset& copy) = delete;
	InputPreset(InputPreset&& move);									// Move - clears source array.
	InputPreset(InputPreset&& move, const char* newName);				// Move with new name.
	virtual ~InputPreset();

	InputPreset& operator=(const InputPreset& copy) = delete;
	InputPreset& operator=(InputPreset&& move);

	void GenerateActions(const GamepadState& gamepadState, const GamepadConfig& gamepadConfig, std::vector<InputAction>& outActions) const;
	void Rename(const std::string& newName);

	bool IsDefault() const;

	static InputPreset CreateDefault();
	static InputPreset DeepCopy(const InputPreset& copy, const char* newName);

protected:

	// Inherited via Serializable
	virtual const FilePath GetFilePath_Internal() override;

	LRT_PROPERTY(InputPreset, std::vector<InputBinding*>, bindings, {});

	static const char* k_defaultName;
};
