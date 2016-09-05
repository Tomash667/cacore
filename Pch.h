#pragma once

//-----------------------------------------------------------------------------
#include "CorePch.h"

//-----------------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define STRICT

//-----------------------------------------------------------------------------
#include <Windows.h>
#include <Shellapi.h>
#include <ctime>
#include <cstdio>
#include <zlib.h>

//-----------------------------------------------------------------------------
// undef some useless macros
#undef far
#undef near
#undef small
#undef DrawText
#undef IN
#undef OUT
#undef ERROR
