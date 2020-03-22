#pragma once

#include "Common.h"
#include "Serializable.h"
#include "GamepadState.h"
#include "InputAction.h"

class GamepadConfig;

// It binds multiple GamepadButtons to multiple InputActions.
// Meaning that if ALL Sources are present then ALL Destinations take place.
// It also is responsible for translating GamepadButton behaviour to a given InputAction.

class InputBinding : public Serializable
{
public:
	using Sources = std::vector<GamepadButtons>;
	using Destinations = std::vector<InputActionKey>;

	InputBinding();	// Empty ctor.
	InputBinding(GamepadButtons source, InputActionKey destination); // Simple 1-1 ctor.
	InputBinding(const Sources& sources, const Destinations& destinations); // General purpose ctor.
	virtual ~InputBinding();

	void GenerateActions(const GamepadState& gamepadState, const GamepadConfig& gamepadConfig, std::vector<InputAction>& outActions) const;

private:

	LRT_PROPERTY(InputBinding, Sources, sources, {});
	LRT_PROPERTY(InputBinding, Destinations, destinations, {});

	static const char* k_propertyName;
	static const int32_t k_mouseMovementMultiplier;
	static const int32_t k_mouseScrollMultiplier;

	// Inherited via Serializable
	virtual const FilePath GetFilePath_Internal() override;
};
