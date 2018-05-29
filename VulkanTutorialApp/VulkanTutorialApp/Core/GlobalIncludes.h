#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
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

#include <chrono>

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

#define JE_Print(text) std::cout << (text)
#define JE_PrintLine(text) std::cout << (text) << std::endl
#define JE_PrintErr(text) std::cerr << (text)
#define JE_PrintLineErr(text) std::cerr << (text) << std::endl

#define JE_VectorSizeBytes(arrayName) (sizeof((arrayName)[0]) * (arrayName).size())

#define JE_VA_ARGS_COUNT(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value