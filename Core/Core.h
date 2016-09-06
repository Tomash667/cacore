#pragma once

#include "Core/CorePch.h"
#include "Core/Base.h"
#include "Core/Config.h"
#include "Core/Container.h"
#include "Core/Crc.h"
#include "Core/CriticalSection.h"
#include "Core/File.h"
#include "Core/Logger.h"
#include "Core/Math.h"
#include "Core/Perlin.h"
#include "Core/Profiler.h"
#include "Core/QuadTree.h"
#include "Core/Stream.h"
#include "Core/Text.h"
#include "Core/Timer.h"
#include "Core/Tokenizer.h"

#ifdef _DEBUG
#	pragma comment(lib, "cacore_debug.lib")
#else
#	pragma comment(lib, "cacore.lib")
#endif
