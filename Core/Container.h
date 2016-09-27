#pragma once

#include "Core/CriticalSection.h"

//-----------------------------------------------------------------------------
// Funkcje obs�uguj�ce vector
//-----------------------------------------------------------------------------
// Usuwanie element�w wektora
template<typename T>
inline void DeleteElements(vector<T>& v)
{
	for(vector<T>::iterator it = v.begin(), end = v.end(); it != end; ++it)
		delete *it;
	v.clear();
}

template<typename T>
inline void DeleteElements(vector<T>* v)
{
	DeleteElements(*v);
}

template<typename T>
inline void DeleteElementsChecked(vector<T>& v)
{
	for(T& item : v)
	{
		assert(item);
		delete item;
	}
	v.clear();
}

// usu� pojedy�czy element z wektora, kolejno�� nie jest wa�na
template<typename T>
inline void DeleteElement(vector<T>& v, T& e)
{
	for(typename vector<T>::iterator it = v.begin(), end = v.end(); it != end; ++it)
	{
		if(e == *it)
		{
			delete *it;
			std::iter_swap(it, end - 1);
			v.pop_back();
			return;
		}
	}

	assert(0 && "Nie znaleziono elementu do usuniecia!");
}

template<typename T>
void DeleteElement(vector<T>* v, T& e)
{
	DeleteElement(*v, e);
}

template<typename T>
inline bool DeleteElementTry(vector<T>& v, T& e)
{
	for(typename vector<T>::iterator it = v.begin(), end = v.end(); it != end; ++it)
	{
		if(e == *it)
		{
			delete *it;
			std::iter_swap(it, end - 1);
			v.pop_back();
			return true;
		}
	}

	return false;
}

template<typename T>
inline void RemoveElement(vector<T>& v, const T& e)
{
	for(typename vector<T>::iterator it = v.begin(), end = v.end(); it != end; ++it)
	{
		if(e == *it)
		{
			std::iter_swap(it, end - 1);
			v.pop_back();
			return;
		}
	}

	assert(0 && "Nie znaleziono elementu do wyrzucenia!");
}

template<typename T>
inline void RemoveElement(vector<T>* v, const T& e)
{
	RemoveElement(*v, e);
}

template<typename T>
inline void RemoveElementOrder(vector<T>& v, const T& e)
{
	for(typename vector<T>::iterator it = v.begin(), end = v.end(); it != end; ++it)
	{
		if(e == *it)
		{
			v.erase(it);
			return;
		}
	}

	assert(0 && "Nie znaleziono elementu do wyrzucenia!");
}

template<typename T>
inline void RemoveElementOrder(vector<T>* v, const T& e)
{
	RemoveElementOrder(*v, e);
}

template<typename T>
inline bool RemoveElementTry(vector<T>& v, const T& e)
{
	for(typename vector<T>::iterator it = v.begin(), end = v.end(); it != end; ++it)
	{
		if(e == *it)
		{
			std::iter_swap(it, end - 1);
			v.pop_back();
			return true;
		}
	}

	return false;
}

template<typename T>
inline bool RemoveElementTry(vector<T>* v, const T& e)
{
	return RemoveElementTry(*v, e);
}

template<typename T>
inline void RemoveElementIndex(vector<T>& v, int index)
{
	std::iter_swap(v.begin() + index, v.end() - 1);
	v.pop_back();
}

template<typename T>
inline void RemoveElementIndexOrder(vector<T>& v, int index)
{
	v.erase(v.begin() + index);
}

template<typename T>
inline bool is_null(const T ptr)
{
	return !ptr;
}

template<typename T>
inline void RemoveNullElements(vector<T>& v)
{
	auto it = std::remove_if(v.begin(), v.end(), is_null<T>);
	auto end = v.end();
	if(it != end)
		v.erase(it, end);
}

template<typename T>
inline void RemoveNullElements(vector<T>* v)
{
	assert(v);
	RemoveNullElements(*v);
}

template<typename T, typename T2>
inline void RemoveElements(vector<T>& v, T2 pred)
{
	auto it = std::remove_if(v.begin(), v.end(), pred);
	auto end = v.end();
	if(it != end)
		v.erase(it, end);
}

template<typename T, typename T2>
inline void RemoveElements(vector<T>* v, T2 pred)
{
	assert(v);
	RemoveElements(*v, pred);
}

template<typename T>
inline T& Add1(vector<T>& v)
{
	v.resize(v.size() + 1);
	return v.back();
}

template<typename T>
inline T& Add1(vector<T>* v)
{
	v->resize(v->size() + 1);
	return v->back();
}

template<typename T>
inline T& Add1(list<T>& v)
{
	v.resize(v.size() + 1);
	return v.back();
}

// zwraca losowy element tablicy
template<typename T>
T& random_item(vector<T>& v)
{
	return v[rand2() % v.size()];
}

template<typename T>
T random_item_pop(vector<T>& v)
{
	uint index = rand2() % v.size();
	T item = v[index];
	v.erase(v.begin() + index);
	return item;
}

template<class T>
inline T RandomItem(std::initializer_list<T> cont)
{
	int index = rand2() % cont.size();
	auto it = cont.begin();
	std::advance(it, index);
	return *it;
}

template<typename T>
inline bool IsInside(const vector<T>& v, const T& elem)
{
	for(vector<T>::const_iterator it = v.begin(), end = v.end(); it != end; ++it)
	{
		if(*it == elem)
			return true;
	}

	return false;
}

template<typename T>
inline bool IsInside(const vector<T>* v, const T& elem)
{
	return IsInside(*v, elem);
}


//-----------------------------------------------------------------------------
// kontener u�ywany na tymczasowe obiekty kt�re s� potrzebne od czasu do czasu
//-----------------------------------------------------------------------------
//#define CHECK_POOL_LEAK
#if defined(CHECK_POOL_LEAK) && !defined(_DEBUG)
#	pragma message("Warning: Disabling CHECK_POOL_LEAK in release mode.")
#	undef CHECK_POOL_LEAK
#endif
template<typename T>
struct ObjectPool
{
	~ObjectPool()
	{
		Cleanup();
	}

	inline T* Get()
	{
		T* t;
		if(pool.empty())
			t = new T;
		else
		{
			t = pool.back();
			pool.pop_back();
		}
		__if_exists(T::OnGet)
		{
			t->OnGet();
		}
		return t;
	}

	inline void VerifyElement(T* t)
	{
		for(T* e : pool)
			assert(t != e);
	}

	inline void CheckDuplicates(vector<T*>& elems)
	{
		for(uint i = 0, count = elems.size(); i < count; ++i)
		{
			T* t = elems[i];
			if(!t)
				continue;
			for(uint j = i + 1; j < count; ++j)
				assert(t != elems[j]);
		}
	}

	inline void Free(T* t)
	{
		assert(t);
#ifdef _DEBUG
		VerifyElement(t);
#endif
		__if_exists(T::OnFree)
		{
			t->OnFree();
		}
#ifdef CHECK_POOL_LEAK
		delete t;
#else
		pool.push_back(t);
#endif
	}

	inline void Free(vector<T*>& elems)
	{
		if(elems.empty())
			return;

		__if_exists(T::OnFree)
		{
			for(T* e : elems)
			{
				assert(e);
				e->OnFree();
			}
		}

#ifdef CHECK_POOL_LEAK
		DeleteElementsChecked(elems);
#else
#ifdef _DEBUG
		CheckDuplicates(elems);
		for(T* e : elems)
		{
			assert(e);
			VerifyElement(e);
		}
#endif
		pool.insert(pool.end(), elems.begin(), elems.end());
		elems.clear();
#endif
	}

	inline void SafeFree(vector<T*>& elems)
	{
#ifdef _DEBUG
		CheckDuplicates(elems);
#endif
		for(T* e : elems)
		{
			if(e)
			{
#ifdef _DEBUG
				VerifyElement(e);
#endif
				__if_exists(T::OnFree)
				{
					e->OnFree();
				}
#ifdef CHECK_POOL_LEAK
				delete e;
#else
				pool.push_back(e);
#endif
			}
		}
		elems.clear();
	}

	inline void Cleanup()
	{
		DeleteElements(pool);
	}

private:
	vector<T*> pool;
};

template<typename T>
class ObjectPoolProxy
{
public:
	static inline T* Get() { return GetPool().Get(); }
	static inline void Free(T* t) { GetPool().Free(t); }
	static inline void Free(vector<T*>& ts) { GetPool().Free(ts); }
	static inline void SafeFree(vector <T*>& ts) { GetPool().SafeFree(ts); }
	static inline void Cleanup() { GetPool().Cleanup(); }
	inline virtual void Free() { Free((T*)this); }

private:
	inline static ObjectPool<T>& GetPool() { static ObjectPool<T> pool; return pool; }
};

// tymczasowe stringi
extern ObjectPool<string> StringPool;
extern ObjectPool<vector<void*> > VectorPool;

//-----------------------------------------------------------------------------
// Lokalny string kt�ry wykorzystuje StringPool
//-----------------------------------------------------------------------------
struct LocalString
{
	LocalString()
	{
		s = StringPool.Get();
		s->clear();
	}

	LocalString(cstring str)
	{
		assert(str);
		s = StringPool.Get();
		*s = str;
	}

	LocalString(cstring str, cstring str_to)
	{
		s = StringPool.Get();
		uint len = str_to - str;
		s->resize(len);
		memcpy((char*)s->data(), str, len);
	}

	LocalString(const string& str)
	{
		s = StringPool.Get();
		*s = str;
	}

	~LocalString()
	{
		StringPool.Free(s);
	}

	inline void operator = (cstring str)
	{
		*s = str;
	}

	inline void operator = (const string& str)
	{
		*s = str;
	}

	inline char at_back(uint offset) const
	{
		assert(offset < s->size());
		return s->at(s->size() - 1 - offset);
	}

	inline void pop(uint count)
	{
		assert(s->size() > count);
		s->resize(s->size() - count);
	}

	inline void operator += (cstring str)
	{
		*s += str;
	}

	inline void operator += (const string& str)
	{
		*s += str;
	}

	inline void operator += (char c)
	{
		*s += c;
	}

	inline operator cstring() const
	{
		return s->c_str();
	}

	inline string& get_ref()
	{
		return *s;
	}

	inline string* get_ptr()
	{
		return s;
	}

	inline string* operator -> ()
	{
		return s;
	}

	inline const string* operator -> () const
	{
		return s;
	}

	inline bool operator == (cstring str) const
	{
		return *s == str;
	}

	inline bool operator == (const string& str) const
	{
		return *s == str;
	}

	inline bool operator == (const LocalString& str) const
	{
		return *s == *str.s;
	}

	inline bool operator != (cstring str) const
	{
		return *s != str;
	}

	inline bool operator != (const string& str) const
	{
		return *s != str;
	}

	inline bool operator != (const LocalString& str) const
	{
		return *s != *str.s;
	}

	inline bool empty() const
	{
		return s->empty();
	}

	inline cstring c_str() const
	{
		return s->c_str();
	}

	inline void clear()
	{
		s->clear();
	}

private:
	string* s;
};

//-----------------------------------------------------------------------------
// Lokalny wektor przechowuj�cy wska�niki
//-----------------------------------------------------------------------------
template<typename T>
struct LocalVector
{
	static_assert(sizeof(T) == sizeof(void*), "LocalVector element must be pointer or have sizeof pointer.");

	LocalVector()
	{
		v = (vector<T>*)VectorPool.Get();
		v->clear();
	}

	explicit LocalVector(vector<T>& v2)
	{
		v = (vector<T>*)VectorPool.Get();
		*v = v2;
	}

	~LocalVector()
	{
		VectorPool.Free((vector<void*>*)v);
	}

	inline operator vector<T>& ()
	{
		return *v;
	}

	inline vector<T>* operator -> ()
	{
		return v;
	}

	inline const vector<T>* operator -> () const
	{
		return v;
	}

	inline T& operator [] (int n)
	{
		return v->at(n);
	}

	inline void Shuffle()
	{
		std::random_shuffle(v->begin(), v->end(), myrand);
	}

private:
	vector<T>* v;
};

template<typename T>
struct LocalVector2
{
	typedef vector<T> Vector;
	typedef Vector* VectorPtr;
	typedef typename Vector::iterator Iter;

	static_assert(sizeof(T) == sizeof(void*), "LocalVector2 element must be pointer or have sizeof pointer.");

	LocalVector2()
	{
		v = (VectorPtr)VectorPool.Get();
		v->clear();
	}

	explicit LocalVector2(Vector& v2)
	{
		v = (VectorPtr*)VectorPool.Get();
		*v = v2;
	}

	~LocalVector2()
	{
		VectorPool.Free((vector<void*>*)v);
	}

	inline void push_back(T e)
	{
		v->push_back(e);
	}

	inline bool empty() const
	{
		return v->empty();
	}

	inline Iter begin()
	{
		return v->begin();
	}

	inline Iter end()
	{
		return v->end();
	}

	inline uint size() const
	{
		return v->size();
	}

	T& random_item()
	{
		return v->at(rand2() % v->size());
	}

	T& operator [] (int n)
	{
		return v->at(n);
	}

	const T& operator [] (int n) const
	{
		return v->at(n);
	}

	inline vector<T>& Get()
	{
		return *v;
	}

	inline void clear()
	{
		v->clear();
	}

private:
	VectorPtr v;
};

template<typename T>
inline T& random_item(LocalVector2<T>& v)
{
	return v.random_item();
}


//-----------------------------------------------------------------------------
// Loop over list and erase elements that returned true
template<typename T, typename Action>
inline void LoopAndRemove(vector<T>& items, Action action)
{
	items.erase(std::remove_if(items.begin(), items.end(), action), items.end());
}

template<typename T>
struct WeightPair
{
	T item;
	int weight;

	WeightPair(T& item, int weight) : item(item), weight(weight) {}
};

template<typename T>
inline T& RandomItemWeight(vector<WeightPair<T>>& items, int max_weight)
{
	int a = rand2() % max_weight, b = 0;
	for(auto& item : items)
	{
		b += item.weight;
		if(a < b)
			return item.item;
	}
	// if it gets here max_count is wrong, return random item
	return random_item(items).item;
}

extern ObjectPool<vector<byte>> BufPool;

template<typename T>
class LocalVector3
{
public:
	struct iterator : std::iterator<std::input_iterator_tag, T>
	{
		friend class LocalVector3;

		inline T& operator * ()
		{
			return v->at(offset);
		}

		inline bool operator == (const iterator& it) const
		{
			assert(it.v == v);
			return offset == it.offset;
		}

		inline bool operator != (const iterator& it) const
		{
			assert(it.v == v);
			return offset != it.offset;
		}

		inline bool operator < (const iterator& it) const
		{
			assert(it.v == v);
			return offset < it.offset;
		}

		inline iterator& operator ++ ()
		{
			++offset;
			return *this;
		}

		inline iterator operator ++ (int)
		{
			iterator it(v, offset);
			++offset;
			return it;
		}

		inline iterator& operator -- ()
		{
			--offset;
			return *this;
		}

		inline iterator& operator + (uint count)
		{
			offset += count;
			return *this;
		}

		inline iterator& operator - (uint count)
		{
			offset -= count;
			return *this;
		}

		inline int operator - (const iterator& it) const
		{
			assert(it.v == v);
			return offset - it.offset;
		}

	private:
		iterator(LocalVector3* v, uint offset) : v(v), offset(offset) {}

		LocalVector3* v;
		uint offset;
	};

	LocalVector3()
	{
		buf = BufPool.Get();
	}

	~LocalVector3()
	{
		buf->clear();
		BufPool.Free(buf);
	}

	inline T& at(uint offset)
	{
		assert(offset < size());
		return ((T*)buf->data())[offset];
	}

	inline T& back()
	{
		assert(!empty());
		return ((T*)buf->data())[size() - 1];
	}

	inline iterator begin()
	{
		return iterator(this, 0);
	}

	inline bool empty() const
	{
		return buf->empty();
	}

	inline iterator end()
	{
		return iterator(this, size());
	}

	void push_back(T& item)
	{
		uint s = buf->size();
		buf->resize(buf->size() + sizeof(T));
		memcpy(buf->data() + s, &item, sizeof(T));
	}

	inline uint size() const
	{
		return buf->size() / sizeof(T);
	}

	template<typename Pred>
	void sort(Pred pred)
	{
		std::sort((T*)buf->data(), (T*)(buf->data() + buf->size()), pred);
	}

private:
	vector<byte>* buf;
};

template<typename T>
inline int GetIndex(const vector<T>& items, const T& item)
{
	int index = 0;
	for(const T& it : items)
	{
		if(it == item)
			return index;
		++index;
	}
	return -1;
}

template<typename T, typename Pred>
inline int GetIndex(const vector<T>& items, Pred pred)
{
	int index = 0;
	for(const T& it : items)
	{
		if(pred(it))
			return index;
		++index;
	}
	return -1;
}

//-----------------------------------------------------------------------------
template<typename T>
inline bool In(T val, const std::initializer_list<T>& lis)
{
	for(T t : lis)
	{
		if(t == val)
			return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
template<typename T>
class SafeVector
{
public:
	SafeVector()
	{
		cs.Create();
	}

	~SafeVector()
	{
		cs.Free();
	}

	inline void Push(T& e)
	{
		StartCriticalSection section(cs);
		v.push_back(e);
	}

	inline T Pop()
	{
		StartCriticalSection section(cs);
		T e = v.back();
		v.pop_back();
		return e;
	}

	inline bool Any()
	{
		return !v.empty();
	}

private:
	vector<T> v;
	CriticalSection cs;
};
