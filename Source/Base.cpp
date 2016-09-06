#include "Pch.h"
#include "Core/Base.h"

static AssertHandler _assert_handler;

void assert_handler(cstring msg, cstring file, uint line)
{
#ifdef _DEBUG
	if(_assert_handler)
		_assert_handler(msg, file, line);
	else
	{
		static wchar_t msg_buf[256];
		static wchar_t file_buf[256];
		mbstowcs(msg_buf, msg, 256);
		mbstowcs(file_buf, file, 256);
		_wassert(msg_buf, file_buf, line);
	}
#endif
}

void set_assert_handler(AssertHandler handler)
{
	_assert_handler = handler;
}

AssertHandler get_assert_handler()
{
	return _assert_handler;
}
