// FTPSync.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>
#include <stack>
#include <locale>
#include <cassert>

#define FTPS_Assert(val) assert(val)

#ifdef _WIN32
#define FTPS_PLATFORM_WIN32
#else
#define FTPS_PLATFORM_LINUX
#endif