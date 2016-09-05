#pragma once

//-----------------------------------------------------------------------------
class CriticalSection
{
	friend class StartCriticalSection;
public:
	inline CriticalSection() : valid(false)
	{
	}
	inline void Create(uint spin_count = 50)
	{
		if(!valid)
		{
			InitializeCriticalSectionAndSpinCount(&cs, 50);
			valid = true;
		}
	}
	inline void Free()
	{
		if(valid)
		{
			DeleteCriticalSection(&cs);
			valid = false;
		}
	}
	inline void Enter()
	{
		assert(valid);
		EnterCriticalSection(&cs);
	}
	inline void Leave()
	{
		assert(valid);
		LeaveCriticalSection(&cs);
	}
private:
	CRITICAL_SECTION cs;
	bool valid;
};

//-----------------------------------------------------------------------------
class StartCriticalSection
{
public:
	inline StartCriticalSection(CRITICAL_SECTION& _cs) : cs(_cs)
	{
		EnterCriticalSection(&cs);
	}
	inline StartCriticalSection(CriticalSection& _cs) : cs(_cs.cs)
	{
		assert(_cs.valid);
		EnterCriticalSection(&cs);
	}
	inline ~StartCriticalSection()
	{
		LeaveCriticalSection(&cs);
	}
private:
	CRITICAL_SECTION& cs;
};
