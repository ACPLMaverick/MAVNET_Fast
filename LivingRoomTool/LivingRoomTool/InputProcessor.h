#pragma once

#include "BaseProcessor.h"
#include "GamepadProcessor.h"
#include "InputPresetManager.h"

#include <qtimer.h>

class InputProcessor : public BaseProcessor
{
public:

	InputProcessor();
	~InputProcessor();

	LRT_DisallowCopy(InputProcessor);

	GamepadProcessor& GetGamepadProcessor() { return m_gamepadProcessor; }
	InputPresetManager& GetInputPresetManager() { return m_inputPresetManager; }

	void AssignPreset(size_t presetIndex);
	void InvalidatePreset();

protected:

	static const size_t k_invalidPresetIndex = static_cast<size_t>(-1);

	// Inherited via BaseProcessor
	virtual void Init_Internal() override;
	virtual void Cleanup_Internal() override;

	void Tick();
	inline void ResolveGamepad(const GamepadDevice& device, const InputPreset& preset, std::vector<InputAction>& outActions);
	inline void ProcessActions();

	static const int32_t k_tickIntervalMs;

	GamepadProcessor m_gamepadProcessor;
	InputPresetManager m_inputPresetManager;

	std::vector<InputAction> m_actionsToTakePerTick;

	QTimer m_qTimer;
	QMetaObject::Connection m_qTimerConnection;

	size_t m_assignedPreset;
};

