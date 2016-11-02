#pragma once

// gdy trzeba co� na chwil� wczyta� to mo�na u�ywa� tego stringa
extern string g_tmp_string;

cstring Format(cstring fmt, ...);
cstring FormatList(cstring fmt, va_list lis);
cstring Upper(cstring str);
void SplitText(char* buf, vector<cstring>& lines);
bool Unescape(const string& str_in, uint pos, uint length, string& str_out);
inline bool Unescape(const string& str_in, string& str_out)
{
	return Unescape(str_in, 0u, str_in.length(), str_out);
}
bool StringInString(cstring s1, cstring s2);
cstring Escape(cstring str, char quote = 0);
cstring Escape(cstring str, string& out, char quote = 0);
cstring EscapeChar(char c);
cstring EscapeChar(char c, string& out);
string* ToString(const wchar_t* str);

// return index of character in cstring
inline int strchr_index(cstring chrs, char c)
{
	int index = 0;
	do
	{
		if(*chrs == c)
			return index;
		++index;
		++chrs;
	} while(*chrs);

	return -1;
}

namespace TextHelper
{
	// parse string to number, return 0-broken, 1-int, 2-float
	int ToNumber(cstring s, __int64& i, float& f);
	bool ToInt(cstring s, int& result);
	bool ToUint(cstring s, uint& result);
	bool ToFloat(cstring s, float& result);
	bool ToBool(cstring s, bool& result);
}

struct Trimmer
{
	bool done;
	inline Trimmer() : done(false) {}
	inline bool operator () (char c) const
	{
		if(done)
			return false;
		return c == ' ';
	}
};

// trim from start
inline string& ltrim(string& str)
{
	str.erase(str.begin(), find_if(str.begin(), str.end(), [](char& ch)->bool { return !isspace(ch); }));
	return str;
}

// trim from end
inline string& rtrim(string& str)
{
	str.erase(find_if(str.rbegin(), str.rend(), [](char& ch)->bool { return !isspace(ch); }).base(), str.end());
	return str;
}

// trim from both ends
inline string& trim(string& str)
{
	return ltrim(rtrim(str));
}

template<typename T, class Pred>
inline void Join(const vector<T>& v, string& s, cstring separator, Pred pred)
{
	if(v.empty())
		return;
	if(v.size() == 1)
		s += pred(v.front());
	else
	{
		for(vector<T>::const_iterator it = v.begin(), end = v.end() - 1; it != end; ++it)
		{
			s += pred(*it);
			s += separator;
		}
		s += pred(*(v.end() - 1));
	}
}

//-----------------------------------------------------------------------------
struct AnyString
{
	inline AnyString(cstring s) : s(s)
	{
		assert(s);
		assert(strlen(s) > 0);
	}
	inline AnyString(const string& str) : s(str.c_str())
	{
		assert(!str.empty());
	}

	cstring s;
};

//-----------------------------------------------------------------------------
struct AnyStringNull
{
	inline AnyStringNull(cstring s) : s(s)
	{
		if(s)
			assert(strlen(s) > 0);
	}
	inline AnyStringNull(const string& str) : s(str.c_str())
	{
		assert(!str.empty());
	}

	cstring s;
};

inline char strchrs(cstring s, cstring chrs)
{
	assert(s && chrs);

	while(true)
	{
		char c = *s++;
		if(c == 0)
			return 0;
		cstring ch = chrs;
		while(true)
		{
			char c2 = *ch++;
			if(c2 == 0)
				break;
			if(c == c2)
				return c;
		}
	}
}

inline char strchr2(char c, cstring chrs)
{
	assert(chrs);

	while(true)
	{
		char c2 = *chrs++;
		if(c2 == 0)
			return 0;
		if(c == c2)
			return c;
	}
}

//-----------------------------------------------------------------------------
struct CstringComparer
{
	inline bool operator() (cstring s1, cstring s2) const
	{
		return _stricmp(s1, s2) > 0;
	}
};
