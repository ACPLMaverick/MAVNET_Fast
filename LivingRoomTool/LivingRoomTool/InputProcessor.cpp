#include "InputProcessor.h"

#include <Windows.h>

InputProcessor::InputProcessor()
	: BaseProcessor()
	, m_gamepadProcessor()
	, m_qTimer(nullptr)
	, m_assignedPreset(k_invalidPresetIndex)
{
}

InputProcessor::~InputProcessor()
{
}

void InputProcessor::AssignPreset(size_t presetIndex)
{
	m_assignedPreset = presetIndex;
}

void InputProcessor::InvalidatePreset()
{
	m_assignedPreset = k_invalidPresetIndex;
}

void InputProcessor::Init_Internal()
{
	m_gamepadProcessor.Init();

	m_qTimerConnection = connect(&m_qTimer, &QTimer::timeout, this, &InputProcessor::Tick);
	m_qTimer.start(k_tickIntervalMs);
}

void InputProcessor::Cleanup_Internal()
{
	m_qTimer.stop();
	disconnect(m_qTimerConnection);

	m_actionsToTakePerTick.clear();

	m_gamepadProcessor.Cleanup();
}

void InputProcessor::Tick()
{
	if (m_assignedPreset == k_invalidPresetIndex)
	{
		return;
	}

	DisableNumlockIfNecessary();

	// Poll all states on every tick.
	m_gamepadProcessor.PollInputStates();

	const size_t devicesNum = m_gamepadProcessor.GetDevicesNum();
	for (size_t i = 0; i < devicesNum; ++i)
	{
		ResolveGamepad(m_gamepadProcessor.GetGamepadDevice(i), m_inputPresetManager.GetPreset(m_assignedPreset), m_actionsToTakePerTick);
	}

	if (m_actionsToTakePerTick.size() > 0)
	{
		ProcessActions();
		m_actionsToTakePerTick.clear();
	}
}

inline void InputProcessor::ResolveGamepad(const GamepadDevice& a_device, const InputPreset& a_preset, std::vector<InputAction>& outActions)
{
	const GamepadState& deviceState = a_device.GetState();

	for (InputBinding* binding : a_preset.Get_bindings())
	{
		binding->GenerateActions(a_device.GetState(), a_device.GetConfig(), outActions);
	}
}

inline void InputProcessor::ProcessActions()
{
	static std::vector<INPUT> s_inputs;

	const size_t numInputs = m_actionsToTakePerTick.size();
	s_inputs.clear();

	INPUT accumulatedMouseMovement = {};

	for (size_t i = 0; i < numInputs; ++i)
	{
		const InputAction& action = m_actionsToTakePerTick[i];

		if (InputActionKeyHelper::IsMouse(action.GetKey()))
		{
			if (InputActionKeyHelper::IsMouseMove(action.GetKey()))
			{
				switch (action.GetKey())
				{
				case InputActionKey::kMouseUp:
					accumulatedMouseMovement.mi.dy += -action.GetInt();
					break;
				case InputActionKey::kMouseDown:
					accumulatedMouseMovement.mi.dy += action.GetInt();
					break;
				case InputActionKey::kMouseLeft:
					accumulatedMouseMovement.mi.dx += -action.GetInt();
					break;
				case InputActionKey::kMouseRight:
					accumulatedMouseMovement.mi.dx += action.GetInt();
					break;
				case InputActionKey::kMouseScrollUp:
				{
					INPUT& input = s_inputs.emplace_back(INPUT());
					ZeroMemory(&input, sizeof(INPUT));
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = MOUSEEVENTF_WHEEL;
					input.mi.mouseData = WHEEL_DELTA * action.GetInt() / 10;
				}
					break;
				case InputActionKey::kMouseScrollDown:
				{
					INPUT& input = s_inputs.emplace_back(INPUT());
					ZeroMemory(&input, sizeof(INPUT));
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = MOUSEEVENTF_WHEEL;
					input.mi.mouseData = WHEEL_DELTA * (-action.GetInt()) / 10;
				}
					break;
				default:
					LRT_Fail();
					break;
				}
			}
			else
			{
				INPUT& input = s_inputs.emplace_back(INPUT());
				ZeroMemory(&input, sizeof(INPUT));

				input.type = INPUT_MOUSE;

				switch (action.GetKey())
				{
				case InputActionKey::kMouseLMB:
					input.mi.dwFlags = action.GetIsPressed() ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
					break;
				case InputActionKey::kMouseMMB:
					input.mi.dwFlags = action.GetIsPressed() ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
					break;
				case InputActionKey::kMouseRMB:
					input.mi.dwFlags = action.GetIsPressed() ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
					break;
				default:
					LRT_Fail();
					break;
				}
			}
		}
		else
		{
			INPUT& input = s_inputs.emplace_back(INPUT());
			ZeroMemory(&input, sizeof(INPUT));

			input.type = INPUT_KEYBOARD;
			input.ki.wScan = m_converter.ToPlatformKey(action.GetKey());
			input.ki.dwFlags = KEYEVENTF_SCANCODE;

			if (action.GetIsPressed() == false)
			{
				input.ki.dwFlags |= KEYEVENTF_KEYUP;
			}
		}
	}

	if (accumulatedMouseMovement.mi.dx != 0
		|| accumulatedMouseMovement.mi.dy != 0)
	{
		accumulatedMouseMovement.type = INPUT_MOUSE;
		accumulatedMouseMovement.mi.dwFlags = MOUSEEVENTF_MOVE;
		s_inputs.push_back(accumulatedMouseMovement);
	}

	UINT sentInput = SendInput(s_inputs.size(), s_inputs.data(), sizeof(INPUT));
	LRT_Assert(sentInput == s_inputs.size());
}

inline void InputProcessor::DisableNumlockIfNecessary()
{
	SHORT numLockState = GetKeyState(VK_NUMLOCK);
	if (numLockState & (1 << 1) != 0)
	{
		// Disable num lock if it is toggled.
		INPUT inputs[2] = {};
		for (size_t i = 0; i < 2; ++i)
		{
			inputs[i].type = INPUT_KEYBOARD;
			inputs[i].ki.wVk = VK_NUMLOCK;
			inputs[i].ki.dwFlags = 0;
		}

		inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
		UINT sentInput = SendInput(2, inputs, sizeof(INPUT));
		LRT_Assert(sentInput == 2);
	}
}

/*
void InputProcessor::Test_EmitInput()
{
	static bool bIsUp = false;

#if 1
	INPUT testInput[4] = {};

	const WORD key = static_cast<WORD>(MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC));

	testInput[0].type = INPUT_KEYBOARD;
	testInput[0].ki.wScan = key;
	testInput[0].ki.dwFlags = KEYEVENTF_SCANCODE;

	testInput[1].type = INPUT_KEYBOARD;
	testInput[1].ki.wVk = key;
	testInput[1].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

	//testInput[2].type = INPUT_MOUSE;
	//testInput[2].mi.dx = 1;
	//testInput[2].mi.dwFlags = MOUSEEVENTF_MOVE;

	//testInput[3].type = INPUT_MOUSE;
	//testInput[3].mi.dy = 1;
	//testInput[3].mi.dwFlags = MOUSEEVENTF_MOVE;

	SendInput(2, testInput, sizeof(INPUT));
#endif

	bIsUp = !bIsUp;
}
*/

const int32_t InputProcessor::k_tickIntervalMs = 8;
