#pragma once

#include "Common.h"
#include "InputAction.h"

class InputBinding;
class GamepadState;
class GamepadConfig;

class InputActionGenerator
{
public:

	InputActionGenerator();
	InputActionGenerator(const InputActionGenerator& other);
	~InputActionGenerator();
	InputActionGenerator& operator=(const InputActionGenerator& other);

	void ResolveMode(const InputBinding& inputBinding);

	void GenerateActions(
		const InputBinding& inputBinding,
		const GamepadState& gamepadState,
		const GamepadConfig& gamepadConfig,
		std::vector<InputAction>& outActions);

private:

	using GenerateActionsFunc = void(InputActionGenerator::*)(const InputBinding& inputBinding, const GamepadState&, const GamepadConfig&, std::vector<InputAction>&);

	void GenerateActions_ButtonToKey(const InputBinding& inputBinding, const GamepadState& gamepadState, const GamepadConfig& gamepadConfig, std::vector<InputAction>& outActions);
	void GenerateActions_AnalogToMouseMove(const InputBinding& inputBinding, const GamepadState& gamepadState, const GamepadConfig& gamepadConfig, std::vector<InputAction>& outActions);
	void GenerateActions_ButtonToMouseMove(const InputBinding& inputBinding, const GamepadState& gamepadState, const GamepadConfig& gamepadConfig, std::vector<InputAction>& outActions);
	void GenerateActions_AnalogToKey(const InputBinding& inputBinding, const GamepadState& gamepadState, const GamepadConfig& gamepadConfig, std::vector<InputAction>& outActions);
	void GenerateActions_Mixed(const InputBinding& inputBinding, const GamepadState& gamepadState, const GamepadConfig& gamepadConfig, std::vector<InputAction>& outActions);

	static bool IsButtonBindingSatisfied(const InputBinding& inputBinding, const GamepadState& gamepadState, const GamepadConfig& gamepadConfig);
	static void SubmitKeyActions(bool value, const InputBinding& inputBinding, std::vector<InputAction>& outActions);
	static void SubmitAnalogActions(int32_t value, const InputBinding& inputBinding, std::vector<InputAction>& outActions);

	GenerateActionsFunc m_generateActionsFunc;
	bool m_bBindingIssued;
};

