#pragma once

#include <cstdint>
#include <assert.h>
#include <vector>
#include <stack>

#include "qdebug.h"

#define LRT_Assert(_predicate_) assert(_predicate_)
#define LRT_Fail() LRT_Assert(false)
#define LRT_CheckHR(_call_)				\
{										\
	HRESULT hr = _call_;				\
	LRT_Assert(hr == S_OK);				\
}

#define LRT_DisallowCopy(_type_)			\
_type_(const _type_&) = delete;				\
_type_& operator=(const _type_&) = delete;

#define LRT_QAddName(_qObject_) _qObject_->setObjectName(QString::fromUtf8(#_qObject_))