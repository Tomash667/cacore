#include "Pch.h"
#include "Base.h"
#include "Container.h"

ObjectPool<string> StringPool;
ObjectPool<vector<void*>> VectorPool;
ObjectPool<vector<byte>> BufPool;
