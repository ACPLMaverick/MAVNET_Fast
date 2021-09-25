#pragma once

#include <cstdint>
#include <assert.h>
#include <vector>
#include <stack>
#include <string>
#include <map>

#include "qdebug.h"

#ifdef NDEBUG
#define LRT_PrintLastError()
#define LRT_PrintHResult(_hr_)
#define LRT_GetLastErrorAsString() ""
#else
extern void LRT_PrintLastError();
extern void LRT_PrintHResult(long hr);
extern std::string LRT_GetLastErrorAsString();
#endif

#define LRT_Assert(_predicate_) assert(_predicate_)
#define LRT_Fail() LRT_Assert(false)
#define LRT_Todo() LRT_Fail()

#ifdef NDEBUG
#define LRT_Verify(_predicate_) _predicate_
#define LRT_CheckHR(_call_) _call_
#else
#define LRT_Verify(_predicate_) LRT_Assert(_predicate_)
#define LRT_CheckHR(_call_)				\
{										\
	HRESULT hr = _call_;				\
	if(hr != S_OK)						\
	{									\
		LRT_PrintHResult((long)hr);		\
	}									\
	LRT_Assert(hr == S_OK);				\
}
#endif


#define LRT_DisallowCopy(_type_)			\
_type_(const _type_&) = delete;				\
_type_& operator=(const _type_&) = delete;

#define LRT_QAddName(_qObject_) _qObject_->setObjectName(QString::fromUtf8(#_qObject_))

#define LRT_SafeDelete(_ptr_)	\
if (_ptr_ != nullptr)			\
{								\
	delete _ptr_;				\
	_ptr_ = nullptr;			\
}

class QListWidget;
namespace LRT_QTHelper
{
	size_t GetQListSelectedIndex(QListWidget* list);

	static const size_t k_invalidIndex = static_cast<size_t>(-1);
}