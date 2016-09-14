#pragma once

extern char BUF[256];

namespace core
{
	namespace io
	{
		// Delete directory.
		bool DeleteDirectory(cstring dir);
		// Check if directory exists.
		bool DirectoryExists(cstring dir);
		// Check if file exists.
		bool FileExists(cstring filename);
		// Find files matching pattern, return false from func to stop.
		bool FindFiles(cstring pattern, const std::function<bool(const WIN32_FIND_DATA&)>& func, bool exclude_special = true);
		// Call ShellExecute on file
		void Execute(cstring file);
	}
}

void Crypt(char* inp, uint inplen, cstring key, uint keylen);

//-----------------------------------------------------------------------------
// Funkcje zapisuj�ce/wczytuj�ce z pliku
//-----------------------------------------------------------------------------
extern DWORD tmp;

bool LoadFileToString(cstring path, string& str);

template<typename T>
inline void WriteString(HANDLE file, const string& s)
{
	assert(s.length() <= std::numeric_limits<T>::max());
	T len = (T)s.length();
	WriteFile(file, &len, sizeof(len), &tmp, nullptr);
	if(len)
		WriteFile(file, s.c_str(), len, &tmp, nullptr);
}

template<typename T>
inline void ReadString(HANDLE file, string& s)
{
	T len;
	ReadFile(file, &len, sizeof(len), &tmp, nullptr);
	if(len)
	{
		s.resize(len);
		ReadFile(file, (char*)s.c_str(), len, &tmp, nullptr);
	}
	else
		s.clear();
}

inline void WriteString1(HANDLE file, const string& s)
{
	WriteString<byte>(file, s);
}

inline void WriteString2(HANDLE file, const string& s)
{
	WriteString<word>(file, s);
}

inline void ReadString1(HANDLE file, string& s)
{
	ReadString<byte>(file, s);
}

inline void ReadString1(HANDLE file)
{
	byte len;
	ReadFile(file, &len, sizeof(len), &tmp, nullptr);
	if(len == 0)
		BUF[0] = 0;
	else
	{
		ReadFile(file, BUF, len, &tmp, nullptr);
		BUF[len] = 0;
	}
}

inline void ReadString2(HANDLE file, string& s)
{
	ReadString<word>(file, s);
}

template<typename COUNT_TYPE, typename STRING_SIZE_TYPE>
inline void WriteStringArray(HANDLE file, vector<string>& strings)
{
	COUNT_TYPE ile = (COUNT_TYPE)strings.size();
	WriteFile(file, &ile, sizeof(ile), &tmp, nullptr);
	for(vector<string>::iterator it = strings.begin(), end = strings.end(); it != end; ++it)
		WriteString<STRING_SIZE_TYPE>(file, *it);
}

template<typename COUNT_TYPE, typename STRING_SIZE_TYPE>
inline void ReadStringArray(HANDLE file, vector<string>& strings)
{
	COUNT_TYPE ile;
	ReadFile(file, &ile, sizeof(ile), &tmp, nullptr);
	strings.resize(ile);
	for(vector<string>::iterator it = strings.begin(), end = strings.end(); it != end; ++it)
		ReadString<STRING_SIZE_TYPE>(file, *it);
}

//-----------------------------------------------------------------------------
class FileReader
{
public:
	FileReader() : file(INVALID_HANDLE_VALUE), own_handle(false)
	{
	}

	explicit FileReader(HANDLE file) : file(file), own_handle(false)
	{
	}

	explicit FileReader(cstring filename) : own_handle(true)
	{
		Open(filename);
	}

	~FileReader()
	{
		if(own_handle && file != INVALID_HANDLE_VALUE)
		{
			CloseHandle(file);
			file = INVALID_HANDLE_VALUE;
		}
	}

	inline bool Open(cstring filename)
	{
		assert(filename);
		file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		own_handle = true;
		return (file != INVALID_HANDLE_VALUE);
	}

	inline bool IsOpen() const
	{
		return file != INVALID_HANDLE_VALUE;
	}

	inline bool Read(void* ptr, uint size)
	{
		ReadFile(file, ptr, size, &tmp, nullptr);
		return size == tmp;
	}

	template<typename T>
	inline bool operator >> (T& a)
	{
		return Read(&a, sizeof(a));
	}

	template<typename T>
	inline T Read()
	{
		T a;
		Read(&a, sizeof(T));
		return a;
	}

	template<typename T>
	inline bool Read(T& a)
	{
		return Read(&a, sizeof(a));
	}

	template<typename T, typename T2>
	inline bool ReadCasted(T2& a)
	{
		T b;
		if(!Read<T>(b))
			return false;
		a = (T2)b;
		return true;
	}

	inline bool ReadStringBUF()
	{
		byte len = Read<byte>();
		if(len == 0)
		{
			BUF[0] = 0;
			return true;
		}
		else
		{
			BUF[len] = 0;
			return Read(BUF, len);
		}
	}

	template<typename T>
	inline void Skip()
	{
		SetFilePointer(file, sizeof(T), nullptr, FILE_CURRENT);
	}

	inline void Skip(int bytes)
	{
		SetFilePointer(file, bytes, nullptr, FILE_CURRENT);
	}

	inline bool ReadString1(string& s)
	{
		byte len;
		if(!Read(len))
			return false;
		s.resize(len);
		return Read((char*)s.c_str(), len);
	}

	inline bool ReadString2(string& s)
	{
		word len;
		if(!Read(len))
			return false;
		s.resize(len);
		return Read((char*)s.c_str(), len);
	}

	inline bool operator >> (string& s)
	{
		return ReadString1(s);
	}

	inline operator bool() const
	{
		return file != INVALID_HANDLE_VALUE;
	}

	inline void ReadToString(string& s)
	{
		DWORD size = GetFileSize(file, nullptr);
		s.resize(size);
		ReadFile(file, (char*)s.c_str(), size, &tmp, nullptr);
		assert(size == tmp);
	}

	template<typename T>
	inline void ReadVector1(vector<T>& v)
	{
		byte count;
		Read(count);
		v.resize(count);
		if(count)
			Read(&v[0], sizeof(T)*count);
	}

	template<typename T>
	inline void ReadVector2(vector<T>& v)
	{
		word count;
		Read(count);
		v.resize(count);
		if(count)
			Read(&v[0], sizeof(T)*count);
	}

	inline uint GetSize() const
	{
		return GetFileSize(file, nullptr);
	}

	HANDLE file;
	bool own_handle;
};

//-----------------------------------------------------------------------------
class FileWriter
{
public:
	FileWriter() : file(INVALID_HANDLE_VALUE), own_handle(true)
	{
	}

	explicit FileWriter(HANDLE file) : file(file), own_handle(false)
	{
	}

	explicit FileWriter(cstring filename) : own_handle(true)
	{
		Open(filename);
	}

	~FileWriter()
	{
		if(own_handle && file != INVALID_HANDLE_VALUE)
		{
			CloseHandle(file);
			file = INVALID_HANDLE_VALUE;
		}
	}

	inline bool Open(cstring filename)
	{
		assert(filename);
		file = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		return (file != INVALID_HANDLE_VALUE);
	}

	inline bool IsOpen() const
	{
		return file != INVALID_HANDLE_VALUE;
	}

	inline void Write(const void* ptr, uint size)
	{
		WriteFile(file, ptr, size, &tmp, nullptr);
		assert(size == tmp);
	}

	template<typename T>
	inline void operator << (const T& a)
	{
		Write(&a, sizeof(a));
	}

	template<typename T>
	inline void Write(const T& a)
	{
		Write(&a, sizeof(a));
	}

	template<typename T, typename T2>
	inline void WriteCasted(const T2& a)
	{
		Write(&a, sizeof(T));
	}

	inline void WriteString1(const string& s)
	{
		int length = s.length();
		assert(length < 256);
		WriteCasted<byte>(length);
		if(length)
			Write(s.c_str(), length);
	}

	inline void WriteString1(cstring str)
	{
		assert(str);
		int length = strlen(str);
		assert(length < 256);
		WriteCasted<byte>(length);
		if(length)
			Write(str, length);
	}

	inline void WriteString2(const string& s)
	{
		int length = s.length();
		assert(length < 256 * 256);
		WriteCasted<word>(length);
		if(length)
			Write(s.c_str(), length);
	}

	inline void WriteString2(cstring str)
	{
		assert(str);
		int length = strlen(str);
		assert(length < 256 * 256);
		Write<word>(length);
		if(length)
			Write(str, length);
	}

	inline void operator << (const string& s)
	{
		WriteString1(s);
	}

	inline void operator << (cstring str)
	{
		assert(str);
		WriteString1(str);
	}

	inline operator bool() const
	{
		return file != INVALID_HANDLE_VALUE;
	}

	inline void Write0()
	{
		WriteCasted<byte>(0);
	}

	template<typename T>
	inline void WriteVector1(const vector<T>& v)
	{
		WriteCasted<byte>(v.size());
		if(!v.empty())
			Write(&v[0], sizeof(T)*v.size());
	}

	template<typename T>
	inline void WriteVector2(const vector<T>& v)
	{
		WriteCasted<word>(v.size());
		if(!v.empty())
			Write(&v[0], sizeof(T)*v.size());
	}

	inline void Flush()
	{
		FlushFileBuffers(file);
	}

	inline uint GetSize() const
	{
		return GetFileSize(file, nullptr);
	}

	HANDLE file;
	bool own_handle;
};

//-----------------------------------------------------------------------------
// Funkcje do �atwiejszej edycji bufora
//-----------------------------------------------------------------------------
template<typename T, typename T2>
inline T* ptr_shift(T2* ptr, uint shift)
{
	return ((T*)(((byte*)ptr) + shift));
}

template<typename T>
void* ptr_shift(T* ptr, uint shift)
{
	return (((byte*)ptr) + shift);
}

template<typename T, typename T2>
inline T& ptr_shiftd(T2* ptr, uint shift)
{
	return *((T*)(((byte*)ptr) + shift));
}
