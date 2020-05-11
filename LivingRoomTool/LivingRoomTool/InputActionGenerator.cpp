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
	const float analogValue = GetAnalogBindingValue(a_inputBinding, a_gamepadState, a_gamepadConfig);
	if (analogValue != 0.0f)
	{
		SubmitMouseActions(analogValue, a_inputBinding, a_gamepadConfig, a_outActions);
	}
}

void InputActionGenerator::GenerateActions_ButtonToMouseMove(const InputBinding& a_inputBinding, const GamepadState& a_gamepadState, const GamepadConfig& a_gamepadConfig, std::vector<InputAction>& a_outActions)
{
	if (IsButtonBindingSatisfied(a_inputBinding, a_gamepadState, a_gamepadConfig))
	{
		SubmitMouseActions(1.0f, a_inputBinding, a_gamepadConfig, a_outActions);
	}
}

void InputActionGenerator::GenerateActions_AnalogToKey(const InputBinding& a_inputBinding, const GamepadState& a_gamepadState, const GamepadConfig& a_gamepadConfig, std::vector<InputAction>& a_outActions)
{
	const float analogValue = GetAnalogBindingValue(a_inputBinding, a_gamepadState, a_gamepadConfig);
	const bool bBindingSatisfied = fabsf(analogValue) > k_analogToKeyThershold;

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

void InputActionGenerator::GenerateActions_Mixed(const InputBinding& a_inputBinding, const GamepadState& a_gamepadState, const GamepadConfig& a_gamepadConfig, std::vector<InputAction>& a_outActions)
{
	GamepadButtons combinedSource = static_cast<GamepadButtons>(0);
	float analogValue = 0.0f;
	const std::vector<GamepadButtons>& sources = a_inputBinding.Get_sources();
	for (GamepadButtons buttons : sources)
	{
		float localAnalogValue = 0.0f;
		switch (buttons)
		{
		case GamepadButtons::kLT:
			analogValue = ApplyDeadzone(a_gamepadState.GetLeftTrigger().Axis, a_gamepadConfig.Get_deadzoneLeftTrigger());
			break;
		case GamepadButtons::kRT:
			analogValue = ApplyDeadzone(a_gamepadState.GetRightTrigger().Axis, a_gamepadConfig.Get_deadzoneRightTrigger());
			break;
		case GamepadButtons::kLThumbLeft:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetLeftThumb().AxisX, a_gamepadConfig.Get_deadzoneLeftThumb(), true);
			break;
		case GamepadButtons::kLThumbUp:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetLeftThumb().AxisY, a_gamepadConfig.Get_deadzoneLeftThumb(), false);
			break;
		case GamepadButtons::kLThumbRight:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetLeftThumb().AxisX, a_gamepadConfig.Get_deadzoneLeftThumb(), false);
			break;
		case GamepadButtons::kLThumbDown:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetLeftThumb().AxisY, a_gamepadConfig.Get_deadzoneLeftThumb(), true);
			break;
		case GamepadButtons::kRThumbLeft:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetRightThumb().AxisX, a_gamepadConfig.Get_deadzoneRightThumb(), true);
			break;
		case GamepadButtons::kRThumbUp:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetRightThumb().AxisY, a_gamepadConfig.Get_deadzoneRightThumb(), false);
			break;
		case GamepadButtons::kRThumbRight:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetRightThumb().AxisX, a_gamepadConfig.Get_deadzoneRightThumb(), false);
			break;
		case GamepadButtons::kRThumbDown:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetRightThumb().AxisY, a_gamepadConfig.Get_deadzoneRightThumb(), true);
			break;
		default:
			combinedSource |= buttons;
			break;
		}
	}


	bool bCanSubmitKeyActions = false;
	bool bCanSubmitMouseActions = false;
	bool bKeyActionSubmitValue = false;

	if (m_bBindingIssued == false)
	{
		if (combinedSource != static_cast<GamepadButtons>(0))
		{
			if (a_gamepadState.AreButtonsHeld(combinedSource))
			{
				bCanSubmitKeyActions = true;
				bKeyActionSubmitValue = true;
				m_bBindingIssued = true;
			}
		}
	}
	else
	{
		bCanSubmitMouseActions = true;

		if (combinedSource != static_cast<GamepadButtons>(0))
		{
			if (a_gamepadState.AreButtonsHeld(combinedSource) == false)
			{
				bCanSubmitKeyActions = true;
				bKeyActionSubmitValue = false;
				m_bBindingIssued = false;
			}
		}
	}


	const std::vector<InputActionKey>& destinations = a_inputBinding.Get_destinations();
	for (InputActionKey actionKey : destinations)
	{
		if (InputActionKeyHelper::IsMouseMove(actionKey))
		{
			if (bCanSubmitMouseActions)
			{
				const int32_t mouseValue = AnalogToMouse(analogValue, actionKey, a_gamepadConfig);
				if (mouseValue != 0)
				{
					a_outActions.push_back(InputAction(actionKey, mouseValue));
				}
			}
		}
		else
		{
			if (bCanSubmitKeyActions)
			{
				a_outActions.push_back(InputAction(actionKey, bKeyActionSubmitValue));
			}
		}
	}
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

float InputActionGenerator::GetAnalogBindingValue(const InputBinding & a_inputBinding, const GamepadState & a_gamepadState, const GamepadConfig & a_gamepadConfig)
{
	float analogValue = 0.0f;
	const std::vector<GamepadButtons>& sources = a_inputBinding.Get_sources();
	for (GamepadButtons buttons : sources)
	{
		float localAnalogValue = 0.0f;
		switch (buttons)
		{
		case GamepadButtons::kLT:
			analogValue = ApplyDeadzone(a_gamepadState.GetLeftTrigger().Axis, a_gamepadConfig.Get_deadzoneLeftTrigger());
			break;
		case GamepadButtons::kRT:
			analogValue = ApplyDeadzone(a_gamepadState.GetRightTrigger().Axis, a_gamepadConfig.Get_deadzoneRightTrigger());
			break;
		case GamepadButtons::kLThumbLeft:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetLeftThumb().AxisX, a_gamepadConfig.Get_deadzoneLeftThumb(), true);
			break;
		case GamepadButtons::kLThumbUp:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetLeftThumb().AxisY, a_gamepadConfig.Get_deadzoneLeftThumb(), false);
			break;
		case GamepadButtons::kLThumbRight:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetLeftThumb().AxisX, a_gamepadConfig.Get_deadzoneLeftThumb(), false);
			break;
		case GamepadButtons::kLThumbDown:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetLeftThumb().AxisY, a_gamepadConfig.Get_deadzoneLeftThumb(), true);
			break;
		case GamepadButtons::kRThumbLeft:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetRightThumb().AxisX, a_gamepadConfig.Get_deadzoneRightThumb(), true);
			break;
		case GamepadButtons::kRThumbUp:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetRightThumb().AxisY, a_gamepadConfig.Get_deadzoneRightThumb(), false);
			break;
		case GamepadButtons::kRThumbRight:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetRightThumb().AxisX, a_gamepadConfig.Get_deadzoneRightThumb(), false);
			break;
		case GamepadButtons::kRThumbDown:
			analogValue = ApplyDeadzoneAndDirection(a_gamepadState.GetRightThumb().AxisY, a_gamepadConfig.Get_deadzoneRightThumb(), true);
			break;
		default:
			LRT_Fail();	// Should not get here.
			break;
		}

		// Always the last valid value is taken into account. 
		// If there are more than one source GamepadButtons, the last non-zero one is valid.
		if (localAnalogValue != 0.0f)
		{
			analogValue = localAnalogValue;
		}
	}

	return analogValue;
}

void InputActionGenerator::SubmitKeyActions(bool value, const InputBinding & a_inputBinding, std::vector<InputAction>& a_outActions)
{
	const std::vector<InputActionKey>& destinations = a_inputBinding.Get_destinations();
	for (InputActionKey actionKey : destinations)
	{
		a_outActions.push_back(InputAction(actionKey, value));
	}
}

void InputActionGenerator::SubmitMouseActions(float a_value, const InputBinding& a_inputBinding, const GamepadConfig& a_gamepadConfig, std::vector<InputAction>& a_outActions)
{
	const std::vector<InputActionKey>& destinations = a_inputBinding.Get_destinations();
	for (InputActionKey actionKey : destinations)
	{
		const int32_t mouseKeyValue = AnalogToMouse(a_value, actionKey, a_gamepadConfig);
		if (mouseKeyValue != 0)
		{
			a_outActions.push_back(InputAction(actionKey, mouseKeyValue));
		}
	}
}

float InputActionGenerator::ApplyDeadzone(float a_rawValue, float a_deadzone)
{
	const float absRawValue = fabsf(a_rawValue);
	const float sign = a_rawValue < 0.0f ? -1.0f : 1.0f;

	if (absRawValue < a_deadzone)
	{
		return 0.0f;
	}
	else
	{
		return ((absRawValue - a_deadzone) / (1.0f - a_deadzone)) * sign;
	}
}

float InputActionGenerator::ApplyDeadzoneAndDirection(float a_rawValue, float a_deadzone, bool a_bIsNegative)
{
	if ((a_bIsNegative && a_rawValue < 0.0f)
		|| (a_bIsNegative == false && a_rawValue > 0.0f))
	{
		return ApplyDeadzone(a_rawValue, a_deadzone);
	}
	else
	{
		return 0.0f;
	}
}

int32_t InputActionGenerator::AnalogToMouse(float a_analogValue, InputActionKey a_mouseKey, const GamepadConfig& a_gamepadConfig)
{
	const float absAnalogValue = fabsf(a_analogValue);

	switch (a_mouseKey)
	{
	case InputActionKey::kMouseUp:
	case InputActionKey::kMouseDown:
		return static_cast<int32_t>(static_cast<float>(a_gamepadConfig.Get_mouseSpeedY() * absAnalogValue));
	case InputActionKey::kMouseLeft:
	case InputActionKey::kMouseRight:
		return static_cast<int32_t>(static_cast<float>(a_gamepadConfig.Get_mouseSpeedX() * absAnalogValue));
	case InputActionKey::kMouseScrollUp:
	case InputActionKey::kMouseScrollDown:
		return static_cast<int32_t>(static_cast<float>(a_gamepadConfig.Get_mouseSpeedScroll() * absAnalogValue));
	default:
		LRT_Fail(); // Should not get here.
		return 0;
	}
}

const float InputActionGenerator::k_analogToKeyThershold = 0.7f;
