#pragma once

//-----------------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define STRICT

//-----------------------------------------------------------------------------
// obs³uga wersji debug
#ifndef _DEBUG
#	define NDEBUG
#	define _SECURE_SCL 0
#	define _HAS_ITERATOR_DEBUGGING 0
#endif

//-----------------------------------------------------------------------------
#include <Windows.h>
#include <cassert>
#include <cstdio>
#include <vector>
#include <list>
#include <algorithm>
#include <limits>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>
#include <functional>

//-----------------------------------------------------------------------------
// Included in Core
#include "Eigen\Core"
#include "Eigen\Geometry"
#include "FastFunc\FastFunc.h"

//-----------------------------------------------------------------------------
using std::string;
using std::vector;
using std::list;
using std::min;
using std::max;

#include "Core/Undefine.h"
