#pragma once

#include "Common.h"
#include "Serializable.h"
#include "GamepadState.h"
#include "InputAction.h"

class GamepadConfig;

// It binds multiple GamepadButtons to multiple InputActions.
// Meaning that if ALL Sources are present then ALL Destinations take place.
// It also is responsible for translating GamepadButton behaviour to a given InputAction.

class InputBinding
{
public:
	using Sources = std::vector<GamepadButtons>;
	using Destinations = std::vector<InputActionKey>;

	void GenerateActions(const GamepadState& gamepadState, const GamepadConfig& gamepadConfig, std::vector<InputAction>& outActions);

private:

	Sources m_sources;
	Destinations m_destinations;

	static const int32_t k_mouseMovementMultiplier;
};
