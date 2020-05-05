#pragma once

#include "Common.h"
#include "Serializable.h"
#include "GamepadState.h"
#include "InputAction.h"
#include "InputActionGenerator.h"

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
	InputBinding(const InputBinding& other);
	InputBinding(InputBinding&& other);
	virtual ~InputBinding();

	InputBinding& operator=(const InputBinding& other);
	InputBinding& operator=(InputBinding&& other);

	void SetSource(GamepadButtons source);
	void SetDestination(InputActionKey destination);
	void SetSources(const Sources& sources);
	void SetDestinations(const Destinations& destinations);
	void SetData(const Sources& sources, const Destinations& destinations);

	void GenerateActions(const GamepadState& gamepadState, const GamepadConfig& gamepadConfig, std::vector<InputAction>& outActions);

private:

	LRT_PROPERTY_READONLY(InputBinding, Sources, sources, {});
	LRT_PROPERTY_READONLY(InputBinding, Destinations, destinations, {});

	InputActionGenerator m_generator;

	static const char* k_propertyName;
	static const int32_t k_mouseMovementMultiplier;
	static const int32_t k_mouseScrollMultiplier;

	// Inherited via Serializable
	virtual const FilePath GetFilePath_Internal() override;
};
