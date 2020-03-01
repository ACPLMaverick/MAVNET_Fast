#pragma once

#include <cstdint>
#include <assert.h>
#include <vector>
#include <stack>

#include "qdebug.h"

#define LRT_Assert(_predicate_) assert(_predicate_)
#define LRT_Fail() LRT_Assert(false)

#define LRT_DisallowCopy(_type_)			\
_type_(const _type_&) = delete;				\
_type_& operator=(const _type_&) = delete;