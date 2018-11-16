#pragma once

#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <string>
#include <cstring>
#include <cmath>

#include <Windows.h>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\compatibility.hpp>

#include <vector>
#include <unordered_map>

#define SR_Assert assert

#define SR_Print(format, ...) std::printf((format), __VA_ARGS__)
#define SR_PrintL(format, ...) std::printf((format), __VA_ARGS__); std::printf("\n")

#define SR_AssertResultGeneral(resultType, resultCorrectValue, funcCall) \
{ \
	resultType result; \
	result = funcCall; \
	SR_Assert(result == resultCorrectValue); \
}
#define SR_AssertGlfw(funcCall) SR_AssertResultGeneral(int, GLFW_TRUE, (funcCall))
#define SR_AssertGlew(funcCall) SR_AssertResultGeneral(GLenum, GLEW_OK, (funcCall))
#define SR_AssertGl(funcCall) SR_AssertResultGeneral(GLint, 0, (funcCall))

#define SR_CleanSimpleObject(objectPtr) \
SR_Assert((objectPtr) != nullptr); \
delete (objectPtr); \
(objectPtr) = nullptr;

#define SR_CleanObject(objectPtr) \
SR_Assert((objectPtr) != nullptr); \
(objectPtr)->Shutdown(); \
delete (objectPtr); \
(objectPtr) = nullptr;