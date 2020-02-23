#pragma once

#include "BaseProcessor.h"

#include <qtimer.h>

class InputProcessor : public BaseProcessor
{
public:

	InputProcessor();

protected:
	// Inherited via BaseProcessor
	virtual void Init_Internal() override;
	virtual void Cleanup_Internal() override;

	void Tick();
	void Test_EmitInput();


	static const int32_t k_tickIntervalMs;

	QTimer m_qTimer;
	QMetaObject::Connection m_qTimerConnection;
};

