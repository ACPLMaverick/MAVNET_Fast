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
	static const uint8_t k_actionKeyToDIK[] = 
	{
		0x00,			// kNone,
						// 
		DIK_ESCAPE,		// kEsc,
		DIK_F1,			// kF1,
		DIK_F2,			// kF2,
		DIK_F3,			// kF3,
		DIK_F4,			// kF4,
		DIK_F5,			// kF5,
		DIK_F6,			// kF6,
		DIK_F7,			// kF7,
		DIK_F8,			// kF8,
		DIK_F9,			// kF9,
		DIK_F10,		// kF10,
		DIK_F11,		// kF11,
		DIK_F12,		// kF12,
						// 
		DIK_GRAVE,		// kGrave,
		DIK_1,			// k1,
		DIK_2,			// k2,
		DIK_3,			// k3,
		DIK_4,			// k4,
		DIK_5,			// k5,
		DIK_6,			// k6,
		DIK_7,			// k7,
		DIK_8,			// k8,
		DIK_9,			// k9,
		DIK_0,			// k0,
		DIK_MINUS,		// kMinus,
		DIK_EQUALS,		// kPlus,
		DIK_LBRACKET,	// kLeftBracket,
		DIK_RBRACKET,	// kRightBracket,
		DIK_SEMICOLON,	// kSemicolon,
		DIK_APOSTROPHE,	// kApostrophe,
		DIK_BACKSLASH,	// kBackslash,
		DIK_COMMA,		// kComma,
		DIK_PERIOD,		// kPeriod,
		DIK_SLASH,		// kSlash,
						// 
		DIK_TAB,		// kTab,
		DIK_CAPSLOCK,	// kCapsLock,
		DIK_LSHIFT,		// kLeftShift,
		DIK_LCONTROL,	// kLeftCtrl,
		DIK_LWIN,		// kSystemKey,
		DIK_LALT,		// kLeftAlt,
		DIK_SPACE,		// kSpace,
		DIK_RALT,		// kRightAlt,
		DIK_RMENU,		// kMenu,
		DIK_RCONTROL,	// kRightCtrl,
		DIK_RSHIFT,		// kRightShift,
		DIK_RETURN,		// kEnter,
		DIK_BACKSPACE,	// kBackspace,
						// 
		DIK_Q,			// kQ,
		DIK_W,			// kW,
		DIK_E,			// kE,
		DIK_R,			// kR,
		DIK_T,			// kT,
		DIK_Y,			// kY,
		DIK_U,			// kU,
		DIK_I,			// kI,
		DIK_O,			// kO,
		DIK_P,			// kP,
		DIK_A,			// kA,
		DIK_S,			// kS,
		DIK_D,			// kD,
		DIK_F,			// kF,
		DIK_G,			// kG,
		DIK_H,			// kH,
		DIK_J,			// kJ,
		DIK_K,			// kK,
		DIK_L,			// kL,
		DIK_Z,			// kZ,
		DIK_X,			// kX,
		DIK_C,			// kC,
		DIK_V,			// kV,
		DIK_B,			// kB,
		DIK_N,			// kN,
		DIK_M,			// kM,
						// 
		DIK_SYSRQ,		// kPrintScreen,
		DIK_SCROLL,		// kScrollLock,
		DIK_PAUSE,		// kPauseBreak,
		DIK_INSERT,		// kInsert,
		DIK_HOME,		// kHome,
		DIK_PGUP,		// kPageUp,
		DIK_DELETE,		// kDelete,
		DIK_END,		// kEnd,
		DIK_PGDN,		// kPageDown,
						// 
		DIK_UPARROW,	// kArrowUp,
		DIK_RIGHTARROW,	// kArrowRight,
		DIK_DOWNARROW,	// kArrowDown,
		DIK_LEFTARROW	// kArrowLeft,
	};

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
				case InputActionKey::kMouseScrollUp:
					input.mi.dwFlags = MOUSEEVENTF_WHEEL;
					input.mi.mouseData = WHEEL_DELTA * action.GetInt();
					break;
				case InputActionKey::kMouseScrollDown:
					input.mi.dwFlags = MOUSEEVENTF_WHEEL;
					input.mi.mouseData = WHEEL_DELTA * (-action.GetInt());
					break;
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
			input.ki.wScan = k_actionKeyToDIK[static_cast<uint8_t>(action.GetKey())];
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
