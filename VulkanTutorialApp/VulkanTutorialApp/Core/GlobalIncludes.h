#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cstdarg>

#include <vector>
#include <map>
#include <stack>
#include <tuple>

#include <cassert>
#define JE_Assert(val) assert(val)
#define JE_AssertVkResult(expr) JE_Assert((expr) == VkResult::VK_SUCCESS)
#define JE_AssertThrow(cond, text) if(!(cond)) throw std::runtime_error(text)
#define JE_AssertThrowDefault(cond) if(!(cond)) throw std::runtime_error("Error!")
#define JE_AssertThrowVkResult(expr) \
if((expr) != VkResult::VK_SUCCESS) \
{ \
	JE_AssertThrow(false, #expr); \
}

#define JE_Print(text) std::cout << (text) << std::endl
#define JE_PrintErr(text) std::cerr << (text) << std::endl

#define JE_VA_ARGS_COUNT(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value