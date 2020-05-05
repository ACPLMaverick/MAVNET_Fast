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

	m_gamepadProcessor.Cleanup();
}

void InputProcessor::Tick()
{
	if (m_assignedPreset == k_invalidPresetIndex)
	{
		return;
	}

	// Poll all states on every tick.
	m_gamepadProcessor.PollInputStates();

	std::vector<InputAction> actionsToTake;

	const size_t devicesNum = m_gamepadProcessor.GetDevicesNum();
	for (size_t i = 0; i < devicesNum; ++i)
	{
		ResolveGamepad(m_gamepadProcessor.GetGamepadDevice(i), m_inputPresetManager.GetPreset(m_assignedPreset), actionsToTake);
	}

	for (const InputAction& action : actionsToTake)
	{
		ProcessAction(action);
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

inline void InputProcessor::ProcessAction(const InputAction & action)
{
	LRT_Todo();
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
