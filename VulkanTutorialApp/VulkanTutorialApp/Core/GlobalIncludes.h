#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <stb_image.h>

#include <tiny_obj_loader.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cstdarg>

#include <vector>
#include <map>
#include <unordered_map>
#include <stack>
#include <set>
#include <tuple>
#include <array>
#include <algorithm>

#include <chrono>

#include <cassert>


typedef uint64_t UidShader;
typedef uint64_t UidMisc;
typedef uint64_t UidEntity;


#define JE_AlignAs(val) alignas(val)
#define JE_Inline inline

#define JE_Debug 1

#define JE_Assert(val, ...) assert(val)
#define JE_AssertVkResult(expr) JE_Assert((expr) == VkResult::VK_SUCCESS)
#define JE_AssertThrow(cond, text) if(!(cond)) throw std::runtime_error(text)
#define JE_AssertThrowDefault(cond) if(!(cond)) throw std::runtime_error("Error!")
#define JE_AssertThrowVkResult(expr) \
if((expr) != VkResult::VK_SUCCESS) \
{ \
	JE_AssertThrow(false, #expr); \
}
#define JE_AssertStatic(val) static_assert(val, "Static assertion failed.")
#define JE_AssertBitCount(val, bitWidth) JE_Assert(IsBitWidthEqualOrLesserThan(val, bitWidth))
#define JE_TODO() JE_Assert(false)

extern bool IsBitWidthEqualOrLesserThan(uint64_t val, uint8_t bitWidth);

#define JE_Print(text) std::cout << (text)
#define JE_PrintLine(text) std::cout << (text) << std::endl
#define JE_PrintWarn(text) JE_Print(text)
#define JE_PrintWarnLine(text) JE_PrintLine(text)
#define JE_PrintErr(text) std::cerr << (text)
#define JE_PrintLineErr(text) std::cerr << (text) << std::endl

#define JE_VectorSizeBytes(arrayName) (sizeof((arrayName)[0]) * (arrayName).size())
#define JE_ArrayLength(arrayName) (sizeof(arrayName)/sizeof(arrayName[0]))
#define JE_FillZeros(objName) (memset(&(objName), 0, sizeof(objName)))

#if OS_LINUX
#define JE_NewAligned(typeName, alignment, ...) (new (aligned_alloc(alignment, sizeof(typeName))) typeName(__VA_ARGS__))
#define JE_DeleteAligned(object, typeName, alignment)	\
{	\
	object->~typeName();	\
	free((void*)object);	\
}
#elif OS_WINDOWS
#define JE_NewAligned(typeName, alignment, ...) (new (_aligned_malloc(sizeof(typeName), alignment)) typeName(__VA_ARGS__))
#define JE_DeleteAligned(object, typeName, alignment)	\
{	\
	object->~typeName();	\
	_aligned_free((void*)object);	\
}
#else
#error "Undefined for this platform."
#endif

#define JE_VA_ARGS_COUNT(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value


#include "Util/GlobalIncludes.h"


#define JE_EnumBegin(enumName) \
enum class enumName : uint8_t			\
{

#define JE_EnumEnd()	\
	,ENUM_SIZE			\
};


#define JE_GetSystemClassName(className) System##className

#define JE_DeclareSystemClass(className) \
class JE_GetSystemClassName(className) : public Util::Singleton<JE_GetSystemClassName(className)>

#define JE_DeclareSystemClassBody(className)					\
friend class Util::Singleton<JE_GetSystemClassName(className)>;	\
	private:													\
		JE_GetSystemClassName(className)() {}					\
		~JE_GetSystemClassName(className)() {}					\

#define JE_DefineSystemClassBody(className)

#define JE_DeclareClientClass(className, parentClassName)		\
class JE_GetSystemClassName(className);							\
class className : public parentClassName

#define JE_DeclareClientClassBody(className, parentClassName)	\
	friend class JE_GetSystemClassName(className);				\
public:															\
	className (const className& other);							\
	className (const className&& otherMove);					\
	~className();												\
	className& operator=(const className& other);				\
	className& operator=(const className&& otherMove);			\
private:														\
	className();												

#define JE_DefineClientClassBody(className, parentClassName)

#define JE_GetApp() Core::HelloTriangle::GetInstance()