#pragma once

#include "Common.h"

#include <qobject.h>

class BaseProcessor : public QObject
{
	Q_OBJECT

public:

	BaseProcessor();

	LRT_DisallowCopy(BaseProcessor);

	void Init();
	void Cleanup();
	bool IsInit() const { return m_bIsInit; }
	void Toggle();

protected:

	virtual void Init_Internal() = 0;
	virtual void Cleanup_Internal() = 0;

	bool m_bIsInit;
};

