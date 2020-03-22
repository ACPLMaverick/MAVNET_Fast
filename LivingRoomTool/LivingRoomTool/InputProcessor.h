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

protected:
	// Inherited via BaseProcessor
	virtual void Init_Internal() override;
	virtual void Cleanup_Internal() override;

	void Tick();

	static const int32_t k_tickIntervalMs;

	GamepadProcessor m_gamepadProcessor;
	InputPresetManager m_inputPresetManager;

	QTimer m_qTimer;
	QMetaObject::Connection m_qTimerConnection;
};

