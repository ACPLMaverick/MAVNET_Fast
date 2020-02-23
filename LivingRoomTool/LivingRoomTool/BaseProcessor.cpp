#include "BaseProcessor.h"

BaseProcessor::BaseProcessor()
	: QObject(nullptr)
	, m_bIsInit(false)
{
}

void BaseProcessor::Init()
{
	if (IsInit())
	{
		LRT_Fail();
		return;
	}

	Init_Internal();
	m_bIsInit = true;
}

void BaseProcessor::Cleanup()
{
	if (IsInit() == false)
	{
		LRT_Fail();
		return;
	}

	m_bIsInit = false;
	Cleanup_Internal();
}

void BaseProcessor::Toggle()
{
	if (IsInit())
	{
		Cleanup();
	}
	else
	{
		Init();
	}
}
