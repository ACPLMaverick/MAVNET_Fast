#include "InputActionGenerator.h"

#include "InputBinding.h"
#include "GamepadState.h"
#include "GamepadConfig.h"

InputActionGenerator::InputActionGenerator()
	: m_generateActionsFunc(&InputActionGenerator::GenerateActions_Mixed)
	, m_bBindingIssued(false)
{
}

InputActionGenerator::InputActionGenerator(const InputActionGenerator & a_other)
	: m_generateActionsFunc(a_other.m_generateActionsFunc)
	, m_bBindingIssued(a_other.m_bBindingIssued)
{
}

InputActionGenerator::~InputActionGenerator()
{
	// Reset issue state on destructor.
	m_bBindingIssued = false;
}

InputActionGenerator& InputActionGenerator::operator=(const InputActionGenerator & a_other)
{
	m_generateActionsFunc = a_other.m_generateActionsFunc;
	m_bBindingIssued = a_other.m_bBindingIssued;

	return *this;
}

void InputActionGenerator::ResolveMode(const InputBinding& a_inputBinding)
{
	bool bHasOnlyButtonsInSources = true;
	bool bHasOnlyKeysInDestinations = true;
	bool bHasOnlyAnalogsInSources = true;
	bool bHasOnlyMouseMoveInDestinations = true;

	const std::vector<GamepadButtons>& sources = a_inputBinding.Get_sources();
	const std::vector<InputActionKey>& destinations = a_inputBinding.Get_destinations();

	for (GamepadButtons button : sources)
	{
		if (bHasOnlyButtonsInSources && GamepadButtonsHelper::IsAnalog(button))
		{
			bHasOnlyButtonsInSources = false;
		}
		else if (bHasOnlyAnalogsInSources && GamepadButtonsHelper::IsAnalog(button) == false)
		{
			bHasOnlyAnalogsInSources = false;
		}
		else if (bHasOnlyButtonsInSources == false && bHasOnlyAnalogsInSources == false)
		{
			break;	// Nothing else to do in this loop.
		}
	}

	for (InputActionKey key : destinations)
	{
		if (bHasOnlyKeysInDestinations && InputActionKeyHelper::IsMouseMove(key))
		{
			bHasOnlyKeysInDestinations = false;
		}
		else if (bHasOnlyMouseMoveInDestinations && InputActionKeyHelper::IsMouseMove(key) == false)
		{
			bHasOnlyMouseMoveInDestinations = false;
		}
		else if (bHasOnlyKeysInDestinations == false && bHasOnlyMouseMoveInDestinations == false)
		{
			break; // Nothing else to do in this loop.
		}
	}

	if (bHasOnlyButtonsInSources && bHasOnlyKeysInDestinations)
	{
		m_generateActionsFunc = &InputActionGenerator::GenerateActions_ButtonToKey;
	}
	else if (bHasOnlyAnalogsInSources && bHasOnlyMouseMoveInDestinations)
	{
		m_generateActionsFunc = &InputActionGenerator::GenerateActions_AnalogToMouseMove;
	}
	else if (bHasOnlyButtonsInSources && bHasOnlyMouseMoveInDestinations)
	{
		m_generateActionsFunc = &InputActionGenerator::GenerateActions_ButtonToMouseMove;
	}
	else if (bHasOnlyAnalogsInSources && bHasOnlyKeysInDestinations)
	{
		m_generateActionsFunc = &InputActionGenerator::GenerateActions_AnalogToKey;
	}
	else
	{
		m_generateActionsFunc = &InputActionGenerator::GenerateActions_Mixed;
	}

	m_bBindingIssued = false;	// Reset the binding issued flag on resolve mode.
}

void InputActionGenerator::GenerateActions(const InputBinding& a_inputBinding, const GamepadState& a_gamepadState, const GamepadConfig& a_gamepadConfig, std::vector<InputAction>& a_outActions)
{
	(this->*m_generateActionsFunc)(a_inputBinding, a_gamepadState, a_gamepadConfig, a_outActions);
}

void InputActionGenerator::GenerateActions_ButtonToKey(const InputBinding& a_inputBinding, const GamepadState& a_gamepadState, const GamepadConfig& a_gamepadConfig, std::vector<InputAction>& a_outActions)
{
	const bool bBindingSatisfied = IsButtonBindingSatisfied(a_inputBinding, a_gamepadState, a_gamepadConfig);
	if (bBindingSatisfied && m_bBindingIssued == false)
	{
		SubmitKeyActions(true, a_inputBinding, a_outActions);
		m_bBindingIssued = true;
	}
	else if (bBindingSatisfied == false && m_bBindingIssued)
	{
		SubmitKeyActions(false, a_inputBinding, a_outActions);
		m_bBindingIssued = false;
	}
}

void InputActionGenerator::GenerateActions_AnalogToMouseMove(const InputBinding& a_inputBinding, const GamepadState& a_gamepadState, const GamepadConfig& a_gamepadConfig, std::vector<InputAction>& a_outActions)
{
	LRT_Todo();
}

void InputActionGenerator::GenerateActions_ButtonToMouseMove(const InputBinding& a_inputBinding, const GamepadState& a_gamepadState, const GamepadConfig& a_gamepadConfig, std::vector<InputAction>& a_outActions)
{
	LRT_Todo();
}

void InputActionGenerator::GenerateActions_AnalogToKey(const InputBinding& a_inputBinding, const GamepadState& a_gamepadState, const GamepadConfig& a_gamepadConfig, std::vector<InputAction>& a_outActions)
{
	LRT_Todo();
}

void InputActionGenerator::GenerateActions_Mixed(const InputBinding& a_inputBinding, const GamepadState& a_gamepadState, const GamepadConfig& a_gamepadConfig, std::vector<InputAction>& a_outActions)
{
	LRT_Todo();
}

bool InputActionGenerator::IsButtonBindingSatisfied(const InputBinding & a_inputBinding, const GamepadState & a_gamepadState, const GamepadConfig & a_gamepadConfig)
{
	const std::vector<GamepadButtons>& sources = a_inputBinding.Get_sources();
	GamepadButtons combinedSource = static_cast<GamepadButtons>(0);
	for (GamepadButtons source : sources)
	{
		combinedSource |= source;
	}

	return a_gamepadState.AreButtonsHeld(combinedSource);
}

void InputActionGenerator::SubmitKeyActions(bool value, const InputBinding & a_inputBinding, std::vector<InputAction>& a_outActions)
{
	const std::vector<InputActionKey>& destinations = a_inputBinding.Get_destinations();
	for (InputActionKey actionKey : destinations)
	{
		a_outActions.push_back(InputAction(actionKey, value));
	}
}

void InputActionGenerator::SubmitAnalogActions(int32_t value, const InputBinding & a_inputBinding, std::vector<InputAction>& a_outActions)
{
	const std::vector<InputActionKey>& destinations = a_inputBinding.Get_destinations();
	for (InputActionKey actionKey : destinations)
	{
		a_outActions.push_back(InputAction(actionKey, value));
	}
}
