#pragma once

//-----------------------------------------------------------------------------
// Typy zmiennych
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int uint;
typedef __int64 int64;
typedef unsigned __int64 uint64;
typedef const char* cstring;

// 64 bit version not working propably anyway
static_assert(sizeof(char) == 1, "Invalid char size.");
static_assert(sizeof(short) == 2, "Invalid short size.");
static_assert(sizeof(int) == 4, "Invalid int size.");
static_assert(sizeof(long) == 4, "Invalid long size.");
static_assert(sizeof(int64) == 8, "Invalid int64 size.");
static_assert(sizeof(float) == 4, "Invalid float size.");
static_assert(sizeof(double) == 8, "Invalid double size.");
static_assert(sizeof(void*) == 4, "Invalid pointer size.");

//-----------------------------------------------------------------------------
// Kolory DWORD
#define BLACK 0xFF000000
#define RED   0xFFFF0000
#define GREEN 0xFF00FF00
#define BLUE  0xFF0000FF
#define WHITE 0xFFFFFFFF

#define COLOR_ARGB(a,r,g,b) ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define COLOR_RGB(r,g,b) COLOR_ARGB(0xFF,r,g,b)
#define COLOR_RGBA(r,g,b,a) COLOR_ARGB(a,r,g,b)

//-----------------------------------------------------------------------------
// Makra
#undef NULL
#define BIT(bit) (1<<(bit))
#define IS_SET(flaga,bit) (((flaga) & (bit)) != 0)
#define IS_CLEAR(flaga,bit) (((flaga) & (bit)) == 0)
#define IS_ALL_SET(flaga,bity) (((flaga) & (bity)) == (bity))
#define SET_BIT(flaga,bit) ((flaga) |= (bit))
#define CLEAR_BIT(flaga,bit) ((flaga) &= ~(bit))
#define SET_BIT_VALUE(flaga,bit,wartos) { if(wartos) SET_BIT(flaga,bit); else CLEAR_BIT(flaga,bit); }
#define COPY_BIT(flaga,flaga2,bit) { if(((flaga2) & (bit)) != 0) SET_BIT(flaga,bit); else CLEAR_BIT(flaga,bit); }
#define FLT10(x) (float(int((x)*10))/10)
#define OR2_EQ(var,val1,val2) (((var) == (val1)) || ((var) == (val2)))
#define OR3_EQ(var,val1,val2,val3) (((var) == (val1)) || ((var) == (val2)) || ((var) == (val3)))
// makro na rozmiar tablicy
template <typename T, size_t N>
char(&_ArraySizeHelper(T(&array)[N]))[N];
#define countof( array ) (sizeof( _ArraySizeHelper( array ) ))
#define random_string(ss) ((cstring)((ss)[rand2()%countof(ss)]))
#ifndef STRING
#	define _STRING(str) #str
#	define STRING(str) _STRING(str)
#endif
#define _JOIN(a,b) a##b
#define JOIN(a,b) _JOIN(a,b)
#define FLT_(x, m) (float(int(x*m))/m)
#define FLT_1(x) FLT_(x, 10)
#define FLT_2(x) FLT_(x, 100)

//-----------------------------------------------------------------------------
// Debugowanie
#ifdef assert
#	undef assert
#endif
typedef void(*AssertHandler)(cstring msg, cstring file, uint line);
void assert_handler(cstring msg, cstring file, uint line);
void set_assert_handler(AssertHandler handler);
AssertHandler get_assert_handler();
#ifdef _DEBUG
#	define DEBUG_DO(x) (x)
#	define C(x) assert(x)
#	define assert(expression) (void)((!!(expression)) || (assert_handler(#expression, __FILE__, (uint)(__LINE__)), 0))
#else
#	define DEBUG_DO(x)
#	define C(x) x
#	define assert(expression) ((void)0)
#endif
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC2__ __FILE__ "("__STR1__(__LINE__)") : "
#ifndef _DEBUG
#	define FIXME __pragma(message(__LOC2__ "error: FIXME in release build!"))
#else
#	define FIXME
#endif

//-----------------------------------------------------------------------------
// Delegates
template<typename T>
using delegate = ssvu::FastFunc<T>;
typedef delegate<void()> VoidDelegate;
typedef delegate<void()> VoidF;
typedef delegate<void(cstring)> PrintMsgFunc;

template<typename T>
struct IAllocator
{
	virtual T* Create() = 0;
	virtual void Destroy(T* item) = 0;
};

template<typename T>
struct IVectorAllocator
{
	virtual void Destroy(vector<T*>& items) = 0;
};

namespace cacore::internal
{
	template<typename T>
	struct StandardAllocator : IAllocator<T>
	{
		inline T* Create()
		{
			return new T;
		}

		inline void Destroy(T* item)
		{
			delete item;
		}
	};
}

//-----------------------------------------------------------------------------
// RAII for simple pointer
template<typename T, typename Allocator = cacore::internal::StandardAllocator<T>>
class Ptr
{
	static_assert(std::is_base_of<IAllocator<T>, Allocator>::value, "Allocator must inherit from IAllocator.");
public:
	inline Ptr(nullptr_t) : ptr(nullptr) 
	{
	}
	inline Ptr(T* ptr) : ptr(ptr)
	{
	}
	inline Ptr()
	{
		ptr = allocator.Create();
	}
	inline ~Ptr()
	{
		if(ptr)
			allocator.Destroy(ptr);
	}
	inline void operator = (T* new_ptr)
	{
		if(ptr)
			allocator.Destroy(ptr);
		ptr = new_ptr;
	}
	inline operator T* ()
	{
		return ptr;
	}
	inline T* operator -> ()
	{
		return ptr;
	}
	inline void Ensure()
	{
		if(!ptr)
			ptr = allocator.Create();
	}
	inline T* Pin()
	{
		T* t = ptr;
		ptr = nullptr;
		return t;
	}
	inline T*& Get()
	{
		return ptr;
	}

private:
	T* ptr;
	Allocator allocator;
};

//-----------------------------------------------------------------------------
// RAII for vector of pointers
template<typename T, typename Allocator>
class VectorPtr
{
public:
	static_assert(std::is_base_of<IVectorAllocator<T>, Allocator>::value, "Allocator must inherit from IVectorAllocator.");

	inline VectorPtr() : pinned(false)
	{

	}

	inline ~VectorPtr()
	{
		if(!pinned)
			allocator.Destroy(items);
	}

	inline vector<T*>* operator -> ()
	{
		return &items;
	}

	inline vector<T*>&& Pin()
	{
		pinned = true;
		return std::move(items);
	}

private:
	vector<T*> items;
	Allocator allocator;
	bool pinned;
};

//-----------------------------------------------------------------------------
// In debug it uses dynamic_cast and asserts if cast is valid
// In release it uses C style cast
template<typename T, typename T2>
inline T checked_cast(T2& a)
{
#ifdef _DEBUG
	T b = dynamic_cast<T>(a);
	assert(b);
#else
	T b = (T)a;
#endif
	return b;
}

//-----------------------------------------------------------------------------
// Offset cast
template<typename T>
inline T& offset_cast(void* data, uint offset)
{
	byte* b = ((byte*)data) + offset;
	return *(T*)b;
}

//-----------------------------------------------------------------------------
// Cast using union
template<typename To, typename From>
inline To union_cast(const From& f)
{
	union
	{
		To to;
		From from;
	} a;

	a.from = f;
	return a.to;
}
