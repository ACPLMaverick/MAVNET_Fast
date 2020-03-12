#pragma once

#ifdef _DEBUG
#ifdef NDEBUG
#undef NDEBUG
#endif
#else
#define NDEBUG
#endif

#include <cstdint>
#include <assert.h>
#include <vector>
#include <stack>
#include <string>

#include "qdebug.h"

#ifndef NDEBUG
extern void LRT_PrintLastError();
extern std::string LRT_GetLastErrorAsString();
#else
#define LRT_PrintLastError()
#define LRT_GetLastErrorAsString() ""
#endif

#define LRT_Assert(_predicate_) assert(_predicate_)
#define LRT_Fail() LRT_Assert(false)

#ifdef NDEBUG
#define LRT_Verify(_predicate_) _predicate_
#else
#define LRT_Verify(_predicate_) LRT_Assert(_predicate_)
#endif

#define LRT_CheckHR(_call_)				\
{										\
	HRESULT hr = _call_;				\
	if(hr != S_OK)						\
	{									\
		LRT_PrintLastError();			\
	}									\
	LRT_Assert(hr == S_OK);				\
}


#define LRT_DisallowCopy(_type_)			\
_type_(const _type_&) = delete;				\
_type_& operator=(const _type_&) = delete;

#define LRT_QAddName(_qObject_) _qObject_->setObjectName(QString::fromUtf8(#_qObject_))