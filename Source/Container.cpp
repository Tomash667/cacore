#include "Pch.h"
#include "Core/Base.h"
#include "Core/Container.h"

ObjectPool<string> StringPool;
ObjectPool<vector<void*>> VectorPool;
ObjectPool<vector<byte>> BufPool;
