#pragma once

#include "BaseProcessor.h"
#include "GamepadProcessor.h"

#include <qtimer.h>

class InputProcessor : public BaseProcessor
{
public:

	InputProcessor();
	~InputProcessor();

	InputProcessor(const InputProcessor&) = delete;
	InputProcessor& operator=(const InputProcessor&) = delete;

	GamepadProcessor& GetGamepadProcessor() { return m_gamepadProcessor; }

protected:
	// Inherited via BaseProcessor
	virtual void Init_Internal() override;
	virtual void Cleanup_Internal() override;

	void Tick();

	static const int32_t k_tickIntervalMs;

	GamepadProcessor m_gamepadProcessor;

	QTimer m_qTimer;
	QMetaObject::Connection m_qTimerConnection;
};

