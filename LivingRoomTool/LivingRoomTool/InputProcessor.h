#pragma once

#include "BaseProcessor.h"
#include "GamepadProcessor.h"
#include "InputPresetManager.h"
#include "InputActionConverter.h"

#include <qtimer.h>

class InputProcessor : public BaseProcessor
{
public:

	using WindowHandle = HWND;

	InputProcessor(WindowHandle windowHandle);
	~InputProcessor();

	LRT_DisallowCopy(InputProcessor);

	GamepadProcessor& GetGamepadProcessor() { return m_gamepadProcessor; }
	InputPresetManager& GetInputPresetManager() { return m_inputPresetManager; }

	void AssignPreset(size_t presetIndex);
	void InvalidatePreset();

protected:

	static const size_t k_invalidPresetIndex = static_cast<size_t>(-1);

	// Inherited via BaseProcessor
	void Init_Internal() override;
	void Cleanup_Internal() override;

	void Tick();
	inline void ResolveGamepad(const GamepadDevice& device, const InputPreset& preset, std::vector<InputAction>& outActions);
	inline bool CanResolveGamepad(const GamepadDevice& device);
	inline void ProcessActions();
	inline void DisableNumlockIfNecessary();

	static bool IsGameOnScreen(WindowHandle myHandle);

	static const int32_t k_tickIntervalMs;

	GamepadProcessor m_gamepadProcessor;
	InputPresetManager m_inputPresetManager;

	WindowHandle m_windowHandle;

	std::vector<InputAction> m_actionsToTakePerTick;

	InputActionConverter m_converter;

	QTimer m_qTimer;
	QMetaObject::Connection m_qTimerConnection;

	size_t m_assignedPreset;
};

