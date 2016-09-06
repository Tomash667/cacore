#pragma once

//-----------------------------------------------------------------------------
// Sta�e
const float PI = 3.14159265358979323846f;
const float SQRT_2 = 1.41421356237f;
const float G = 9.8105f;
const float MAX_ANGLE = PI - FLT_EPSILON;

//-----------------------------------------------------------------------------
// Generator liczb pseudolosowych (z MSVC CRT)
#pragma deprecated (rand)
struct RNG
{
	uint val;

	RNG() : val(1)
	{
	}

	inline int rand2()
	{
		val = val * 214013L + 2531011L;
		return ((val >> 16) & 0x7fff);
	}

	inline int rand2_tmp()
	{
		int tval = val * 214013L + 2531011L;
		return ((tval >> 16) & 0x7fff);
	}
};
extern RNG _RNG;

inline int rand2()
{
	return _RNG.rand2();
}
inline int rand2(int a)
{
	assert(a > 0);
	return _RNG.rand2() % a;
}
inline void srand2(int x)
{
	_RNG.val = x;
}
inline uint rand_r2()
{
	return _RNG.val;
}
inline int rand2_tmp()
{
	return _RNG.rand2_tmp();
}
// u�ywane dla random_shuffle
inline int myrand(int n)
{
	return rand2() % n;
}

// Wrap value around range
inline float wrap(float f, float range = PI * 2)
{
	int n = (int)floor(f / range);
	return f - range * n;
}

//-----------------------------------------------------------------------------
// Funkcje matematyczne
template<typename T>
inline void MinMax(T a, T b, T& _min, T& _max)
{
	if(a > b)
	{
		_min = b;
		_max = a;
	}
	else
	{
		_min = a;
		_max = b;
	}
}

template<typename T>
inline T clamp(T f, T _min, T _max)
{
	if(f > _max)
		return _max;
	else if(f < _min)
		return _min;
	else
		return f;
}

// losowa liczba z przedzia�u <0,1>
inline float random()
{
	return (float)rand2() / RAND_MAX;
}

// losowa liczba z przedzia�u <0,a>
template<typename T>
inline T random(T a)
{
	assert(a >= 0);
	return rand2(a + 1);
}
template<>
inline float random(float a)
{
	return ((float)rand2() / RAND_MAX)*a;
}
// losowa liczba z przedzia�u <a,b>
template<typename T>
inline T random(T a, T b)
{
	assert(b >= a);
	return rand2() % (b - a + 1) + a;
}
template<>
inline float random(float a, float b)
{
	assert(b >= a);
	return ((float)rand2() / RAND_MAX)*(b - a) + a;
}

inline float random_part(int parts)
{
	return 1.f / parts * (rand2() % parts);
}

inline float random_normalized(float val)
{
	return (random(-val, val) + random(-val, val)) / 2;
}

template<typename T>
inline T Chance(int c, T a, T b)
{
	return (rand2() % c == 0 ? a : b);
}

template<typename T>
inline T Chance3(int chance_a, int chance_b, int chance_c, T a, T b, T c)
{
	int ch = rand2() % (chance_a + chance_b + chance_c);
	if(ch < chance_a)
		return a;
	else if(ch < chance_a + chance_b)
		return b;
	else
		return c;
}

// minimum z 3 argument�w
template<typename T>
inline T min3(T a, T b, T c)
{
	return min(a, min(b, c));
}

// dystans pomi�dzy punktami 2d
template<typename T>
inline T distance(T x1, T y1, T x2, T y2)
{
	T x = abs(x1 - x2);
	T y = abs(y1 - y2);
	return sqrt(x*x + y*y);
}

inline float distance_sqrt(float x1, float y1, float x2, float y2)
{
	float x = abs(x1 - x2),
		y = abs(y1 - y2);
	return x*x + y*y;
}

float angle(float x1, float y1, float x2, float y2);

inline float angle_dif(float a, float b)
{
	assert(a >= 0.f && a < PI * 2 && b >= 0.f && b < PI * 2);
	return min((2 * PI) - abs(a - b), abs(b - a));
}

inline bool equal(float a, float b)
{
	return abs(a - b) < std::numeric_limits<float>::epsilon();
}

inline bool equal(float a, float b, float e)
{
	return abs(a - b) < e;
}

inline bool not_zero(float a)
{
	return abs(a) >= std::numeric_limits<float>::epsilon();
}

inline bool is_zero(float a)
{
	return abs(a) < std::numeric_limits<float>::epsilon();
}

template<typename T>
inline T sign(T f)
{
	if(f > 0)
		return 1;
	else if(f < 0)
		return -1;
	else
		return 0;
}

inline float lerp(float a, float b, float t)
{
	return (b - a)*t + a;
}

inline int lerp(int a, int b, float t)
{
	return int(t*(b - a)) + a;
}

// W kt�r� stron� trzeba si� obr�ci� �eby by�o najszybciej
float shortestArc(float a, float b);

// Interpolacja k�t�w
void lerp_angle(float& angle, float from, float to, float t);

template<typename T>
inline bool in_range(__int64 value)
{
	return value >= std::numeric_limits<T>::min() && value <= std::numeric_limits<T>::max();
}

template<typename T>
inline bool in_range(T v, T left, T right)
{
	return (v >= left && v <= right);
}

template<typename T>
inline bool in_range2(T left, T a, T b, T right)
{
	return (a >= left && b >= a && b <= right);
}

inline float slerp(float a, float b, float t)
{
	float angle = shortestArc(a, b);
	return a + angle * t;
}

inline int count_bits(int i)
{
	// It's write-only code. Just put a comment that you are not meant to understand or maintain this code, just worship the gods that revealed it to mankind.
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

inline float inf()
{
	return std::numeric_limits<float>::infinity();
}

inline float ToRadians(float degrees)
{
	return degrees * PI / 180;
}

inline float ToDegrees(float radians)
{
	return radians * 180 / PI;
}

inline bool is_pow2(int x)
{
	return ((x > 0) && ((x & (x - 1)) == 0));
}

inline int roundi(float value)
{
	return (int)round(value);
}

//-----------------------------------------------------------------------------
struct INT2;
struct VEC2;
struct VEC3;
struct VEC4;

//-----------------------------------------------------------------------------
// Punkt na liczbach ca�kowitych
//-----------------------------------------------------------------------------
struct INT2
{
	typedef Eigen::Vector2i Type;

	union
	{
		Type ev;
		struct
		{
			int x;
			int y;
		};
		int array[2];
	};

	inline INT2() {}
	inline explicit INT2(int v) : x(v), y(v) {}
	inline INT2(int x, int y) : x(x), y(y) {}
	inline INT2(const INT2& v) : x(v.x), y(v.y) {}
	inline INT2(const Type& ev) : ev(ev) {}
	explicit INT2(const VEC2& v);
	explicit INT2(const VEC3& v);

	inline INT2& operator = (const INT2& v)
	{
		ev = v.ev;
		return *this;
	}

	inline operator int* ()
	{
		return array;
	}

	inline operator const int* () const
	{
		return array;
	}

	inline int operator ()(int _shift) const
	{
		return x + y * _shift;
	}

	inline int& operator [](int index)
	{
		assert(in_range(index, 0, 1));
		return array[index];
	}

	inline int operator [](int index) const
	{
		assert(in_range(index, 0, 1));
		return array[index];
	}

	inline bool operator == (const INT2& i) const
	{
		return (x == i.x && y == i.y);
	}

	inline bool operator != (const INT2& i) const
	{
		return (x != i.x || y != i.y);
	}

	inline const INT2& operator + () const
	{
		return *this;
	}

	inline INT2 operator - () const
	{
		return INT2(-x, -y);
	}

	inline INT2 operator + (const INT2& xy) const
	{
		return INT2(x + xy.x, y + xy.y);
	}

	inline INT2 operator - (const INT2& xy) const
	{
		return INT2(x - xy.x, y - xy.y);
	}

	inline INT2 operator * (int a) const
	{
		return INT2(x*a, y*a);
	}

	inline INT2 operator * (float a) const
	{
		return INT2(int(a*x), int(a*y));
	}

	inline INT2 operator / (int a) const
	{
		return INT2(x / a, y / a);
	}

	inline void operator += (const INT2& xy)
	{
		x += xy.x;
		y += xy.y;
	}

	inline void operator -= (const INT2& xy)
	{
		x -= xy.x;
		y -= xy.y;
	}

	inline void operator *= (int a)
	{
		x *= a;
		y *= a;
	}

	inline void operator /= (int a)
	{
		x /= a;
		y /= a;
	}

	inline int lerp(float t) const
	{
		return int(t*(y - x)) + x;
	}

	inline int random() const
	{
		return ::random(x, y);
	}
};

static_assert(sizeof(INT2) == sizeof(int) * 2, "INT2 size mismatch.");

inline int random(const INT2& a)
{
	return random(a.x, a.y);
}

inline INT2 random(const INT2& a, const INT2& b)
{
	return INT2(random(a.x, b.x), random(a.y, b.y));
}

inline int clamp(int f, const INT2& i)
{
	if(f > i.y)
		return i.y;
	else if(f < i.x)
		return i.x;
	else
		return f;
}

inline INT2 lerp(const INT2& i1, const INT2& i2, float t)
{
	return INT2((int)lerp(float(i1.x), float(i2.x), t), (int)lerp(float(i1.y), float(i2.y), t));
}

inline INT2 Min(const INT2& i1, const INT2& i2)
{
	return INT2(min(i1.x, i2.x), min(i1.y, i2.y));
}

inline INT2 Max(const INT2& i1, const INT2& i2)
{
	return INT2(max(i1.x, i2.x), max(i1.y, i2.y));
}

inline int distance(const INT2& pt1, const INT2& pt2)
{
	return abs(pt1.x - pt2.x) + abs(pt1.y - pt2.y);
}

inline float distance_float(const INT2& pt1, const INT2& pt2)
{
	return abs(float(pt1.x) - float(pt2.x)) + abs(float(pt1.y) - float(pt2.y));
}

// INT2 pod
struct INT2P
{
	int x, y;
};

//-----------------------------------------------------------------------------
// Punkt na liczbach ca�kowitych dodatnich
//-----------------------------------------------------------------------------
struct UINT2
{
	uint x, y;

	UINT2() {}
	UINT2(uint _x, uint _y) : x(_x), y(_y) {}
	UINT2(const UINT2& u) : x(u.x), y(u.y) {}
};

// UINT2 pod
struct UINT2P
{
	uint x, y;
};

//-----------------------------------------------------------------------------
// Prostok�t
//-----------------------------------------------------------------------------
struct Rect
{
	int minx, miny, maxx, maxy;

	Rect() {}
	Rect(int minx, int miny, int maxx, int maxy) : minx(minx), miny(miny), maxx(maxx), maxy(maxy)
	{
		assert(minx <= maxx && miny <= maxy);
	}
};

//-----------------------------------------------------------------------------
// Funkcje VEC2
//-----------------------------------------------------------------------------
struct VEC2
{
	typedef Eigen::Vector2f Type;

	union
	{
		Type ev;
		struct
		{
			float x;
			float y;
		};
		float array[2];
	};

	inline VEC2() {}
	inline VEC2(float x, float y) : x(x), y(y) {}
	inline VEC2(const VEC2& v) : x(v.x), y(v.y) {}
	inline explicit VEC2(const Type& ev) : ev(ev) {}

	inline VEC2& operator = (const VEC2& v)
	{
		ev = v.ev;
		return *this;
	}

	inline operator float* ()
	{
		return array;
	}

	inline operator const float* () const
	{
		return array;
	}

	inline bool operator == (const VEC2& v) const
	{
		return ev == v.ev;
	}

	inline bool operator != (const VEC2& v) const
	{
		return ev != v.ev;
	}

	inline float& operator [](int index)
	{
		assert(in_range(index, 0, 1));
		return array[index];
	}

	inline float operator [](int index) const
	{
		assert(in_range(index, 0, 1));
		return array[index];
	}

	inline const VEC2& operator + () const
	{
		return *this;
	}

	inline VEC2 operator - () const
	{
		return VEC2(-ev);
	}

	inline VEC2 operator + (const VEC2& v) const
	{
		return VEC2(ev + v.ev);
	}

	inline VEC2 operator - (const VEC2& v) const
	{
		return VEC2(ev - v.ev);
	}

	inline VEC2 operator * (float f) const
	{
		return VEC2(ev * f);
	}

	inline VEC2 operator / (float f) const
	{
		return VEC2(ev / f);
	}

	inline void operator += (const VEC2& v)
	{
		ev += v.ev;
	}

	inline void operator -= (const VEC2& v)
	{
		ev -= v.ev;
	}

	inline void operator *= (float f)
	{
		ev *= f;
	}

	inline void operator /= (float f)
	{
		ev /= f;
	}
	
	inline float dot(const VEC2& v) const
	{
		return ev.dot(v.ev);
	}

	inline float length_sq() const
	{
		return ev.squaredNorm();
	}

	inline VEC2 normalize() const
	{
		return VEC2(ev.normalized());
	}

	inline void normalize_me()
	{
		ev.normalize();
	}

	inline float random() const
	{
		return ::random(x, y);
	}

	inline VEC2 wrap() const
	{
		return VEC2(::wrap(x), ::wrap(y));
	}
};

static_assert(sizeof(VEC2) == sizeof(float) * 2, "VEC2 size mismatch.");

inline VEC2 random_circle_pt(float r)
{
	float a = random(),
		b = random();
	if(b < a)
		std::swap(a, b);
	return VEC2(b*r*cos(2 * PI*a / b), b*r*sin(2 * PI*a / b));
}

inline void MinMax(const VEC2& a, const VEC2& b, VEC2& _min, VEC2& _max)
{
	MinMax(a.x, b.x, _min.x, _max.x);
	MinMax(a.y, b.y, _min.y, _max.y);
}

inline VEC2 clamp(const VEC2& v, const VEC2& _min, const VEC2& _max)
{
	return VEC2(clamp(v.x, _min.x, _max.x), clamp(v.y, _min.y, _max.y));
}

inline float clamp(float f, const VEC2& v)
{
	if(f > v.y)
		return v.y;
	else if(f < v.x)
		return v.x;
	else
		return f;
}

inline VEC2 clamp(const VEC2& v)
{
	return VEC2(clamp(v.x, 0.f, 1.f),
		clamp(v.y, 0.f, 1.f));
}

inline VEC2 random(const VEC2& vmin, const VEC2& vmax)
{
	return VEC2(random(vmin.x, vmax.x), random(vmin.y, vmax.y));
}

inline VEC2 random_VEC2(float a, float b)
{
	return VEC2(random(a, b), random(a, b));
}

inline float distance(const VEC2& p1, const VEC2& p2)
{
	float x = abs(p1.x - p2.x);
	float y = abs(p1.y - p2.y);
	return sqrt(x*x + y*y);
}

inline float distance_sqrt(const VEC2& p1, const VEC2& p2)
{
	float x = abs(p1.x - p2.x);
	float y = abs(p1.y - p2.y);
	return x*x + y*y;
}

inline float angle(const VEC2& v1, const VEC2& v2)
{
	return angle(v1.x, v1.y, v2.x, v2.y);
}

// zwraca k�t taki jak w system.txt
inline float lookat_angle(const VEC2& v1, const VEC2& v2)
{
	return wrap(-angle(v1, v2) - PI / 2);
}

inline bool equal(const VEC2& v1, const VEC2& v2)
{
	return equal(v1.x, v2.x) && equal(v1.y, v2.y);
}

inline void Min(const VEC2& v1, const VEC2& v2, VEC2& out)
{
	out.x = min(v1.x, v2.x);
	out.y = min(v1.y, v2.y);
}

inline void Max(const VEC2& v1, const VEC2& v2, VEC2& out)
{
	out.x = max(v1.x, v2.x);
	out.y = max(v1.y, v2.y);
}

struct VEC2P
{
	float x, y;
};

//-----------------------------------------------------------------------------
// Funkcje VEC3
//-----------------------------------------------------------------------------
struct VEC3
{
	typedef Eigen::Vector3f Type;

	union
	{
		Type ev;
		struct
		{
			float x;
			float y;
			float z;
		};
		float array[3];
	};

	inline VEC3() {}
	inline VEC3(float x, float y, float z) : x(x), y(y), z(z) {}
	inline VEC3(const VEC3& v) : x(v.x), y(v.y), z(v.z) {}
	inline explicit VEC3(const Type& ev) : ev(ev) {}

	inline VEC3& operator = (const VEC3& v)
	{
		ev = v.ev;
		return *this;
	}

	inline operator float* ()
	{
		return array;
	}

	inline operator const float* () const
	{
		return array;
	}

	inline bool operator == (const VEC3& v) const
	{
		return ev == v.ev;
	}

	inline bool operator != (const VEC3& v) const
	{
		return ev != v.ev;
	}

	inline float& operator [](int index)
	{
		assert(in_range(index, 0, 2));
		return array[index];
	}

	inline float operator [](int index) const
	{
		assert(in_range(index, 0, 2));
		return array[index];
	}

	inline const VEC3& operator + () const
	{
		return *this;
	}

	inline VEC3 operator - () const
	{
		return VEC3(-ev);
	}

	inline VEC3 operator + (const VEC3& v) const
	{
		return VEC3(ev + v.ev);
	}

	inline VEC3 operator - (const VEC3& v) const
	{
		return VEC3(ev - v.ev);
	}

	inline VEC3 operator * (float f) const
	{
		return VEC3(ev * f);
	}

	inline VEC3 operator / (float f) const
	{
		return VEC3(ev / f);
	}

	inline void operator += (const VEC3& v)
	{
		ev += v.ev;
	}

	inline void operator -= (const VEC3& v)
	{
		ev -= v.ev;
	}

	inline void operator *= (float f)
	{
		ev *= f;
	}

	inline void operator /= (float f)
	{
		ev /= f;
	}

	inline VEC3 cross(const VEC3& v) const
	{
		return VEC3(ev.cross(v.ev));
	}

	inline float dot(const VEC3& v) const
	{
		return ev.dot(v.ev);
	}
};

static_assert(sizeof(VEC3) == sizeof(float) * 3, "VEC3 size mismatch.");

inline void MinMax(const VEC3& a, const VEC3& b, VEC3& _min, VEC3& _max)
{
	MinMax(a.x, b.x, _min.x, _max.x);
	MinMax(a.y, b.y, _min.y, _max.y);
	MinMax(a.z, b.z, _min.z, _max.z);
}

inline VEC3 clamp(const VEC3& v, const VEC3& _min, const VEC3& _max)
{
	return VEC3(clamp(v.x, _min.x, _max.x), clamp(v.y, _min.y, _max.y), clamp(v.z, _min.z, _max.z));
}

inline VEC3 clamp(const VEC3& v)
{
	return VEC3(clamp(v.x, 0.f, 1.f),
		clamp(v.y, 0.f, 1.f),
		clamp(v.z, 0.f, 1.f));
}

inline VEC3 random(const VEC3& vmin, const VEC3& vmax)
{
	return VEC3(random(vmin.x, vmax.x), random(vmin.y, vmax.y), random(vmin.z, vmax.z));
}

inline VEC3 random_VEC3(float a, float b)
{
	return VEC3(random(a, b), random(a, b), random(a, b));
}

inline float distance2d(const VEC3& v1, const VEC3& v2)
{
	float x = abs(v1.x - v2.x),
		z = abs(v1.z - v2.z);
	return sqrt(x*x + z*z);
}

// dystans pomi�dzy punktami 3d
inline float distance(const VEC3& v1, const VEC3& v2)
{
	float x = abs(v1.x - v2.x),
		y = abs(v1.y - v2.y),
		z = abs(v1.z - v2.z);
	return sqrt(x*x + y*y + z*z);
}

inline float distance_sqrt(const VEC3& v1, const VEC3& v2)
{
	float x = abs(v1.x - v2.x),
		y = abs(v1.y - v2.y),
		z = abs(v1.z - v2.z);
	return x*x + y*y + z*z;
}

inline float angle2d(const VEC3& v1, const VEC3& v2)
{
	return angle(v1.x, v1.z, v2.x, v2.z);
}

inline float lookat_angle(const VEC3& v1, const VEC3& v2)
{
	return wrap(-angle2d(v1, v2) - PI / 2);
}

inline bool equal(const VEC3& v1, const VEC3& v2)
{
	return equal(v1.x, v2.x) && equal(v1.y, v2.y) && equal(v1.z, v2.z);
}

inline void Min(const VEC3& v1, const VEC3& v2, VEC3& out)
{
	out.x = min(v1.x, v2.x);
	out.y = min(v1.y, v2.y);
	out.z = min(v1.z, v2.z);
}

inline void Max(const VEC3& v1, const VEC3& v2, VEC3& out)
{
	out.x = max(v1.x, v2.x);
	out.y = max(v1.y, v2.y);
	out.z = max(v1.z, v2.z);
}

inline VEC3 VEC3_x0y(const VEC2& _v, float y = 0.f)
{
	return VEC3(_v.x, y, _v.y);
}

inline VEC2 ToVEC2(const VEC3& v)
{
	return VEC2(v.x, v.z);
}

// VEC3 POD
struct VEC3P
{
	float x, y, z;

	inline void Build(const VEC3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	inline operator VEC3 () const
	{
		return VEC3(x, y, z);
	}
};

//-----------------------------------------------------------------------------
// Funkcje VEC4
//-----------------------------------------------------------------------------
struct VEC4
{
	typedef Eigen::Vector4f Type;
	union
	{
		Type ev;
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		float array[4];
	};

	inline VEC4() {}
	inline VEC4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	inline VEC4(const VEC4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
	inline explicit VEC4(const VEC3& v, float w = 1.f) : x(v.x), y(v.y), z(v.z), w(w) {}
	inline explicit VEC4(const Type& ev) : ev(ev) {}

	inline VEC4& operator = (const VEC4& v)
	{
		ev = v.ev;
		return *this;
	}

	inline operator float* ()
	{
		return array;
	}

	inline operator const float* () const
	{
		return array;
	}

	inline bool operator == (const VEC4& v) const
	{
		return ev == v.ev;
	}

	inline bool operator != (const VEC4& v) const
	{
		return ev != v.ev;
	}

	inline float& operator [](int index)
	{
		assert(in_range(index, 0, 3));
		return array[index];
	}

	inline float operator [](int index) const
	{
		assert(in_range(index, 0, 3));
		return array[index];
	}

	inline const VEC4& operator + () const
	{
		return *this;
	}

	inline VEC4 operator - () const
	{
		return VEC4(-ev);
	}

	inline VEC4 operator + (const VEC4& v) const
	{
		return VEC4(ev + v.ev);
	}

	inline VEC4 operator - (const VEC4& v) const
	{
		return VEC4(ev - v.ev);
	}

	inline VEC4 operator * (float f) const
	{
		return VEC4(ev * f);
	}

	inline VEC4 operator / (float f) const
	{
		return VEC4(ev / f);
	}

	inline void operator += (const VEC4& v)
	{
		ev += v.ev;
	}

	inline void operator -= (const VEC4& v)
	{
		ev -= v.ev;
	}

	inline void operator *= (float f)
	{
		ev *= f;
	}

	inline void operator /= (float f)
	{
		ev /= f;
	}
};

static_assert(sizeof(VEC4) == sizeof(float) * 4, "VEC4 size mismatch.");

inline VEC4 clamp(const VEC4& v)
{
	return VEC4(clamp(v.x, 0.f, 1.f),
		clamp(v.y, 0.f, 1.f),
		clamp(v.z, 0.f, 1.f),
		clamp(v.w, 0.f, 1.f));
}

inline bool equal(const VEC4& v1, const VEC4& v2)
{
	return equal(v1.x, v2.x) && equal(v1.y, v2.y) && equal(v1.z, v2.z) && equal(v1.w, v2.w);
}

//-----------------------------------------------------------------------------
// Prostok�t na int
//-----------------------------------------------------------------------------
struct IBOX2D
{
	INT2 p1, p2;

	IBOX2D() {}
	IBOX2D(int x1, int y1, int x2, int y2) : p1(x1, y1), p2(x2, y2) {}

	inline void Set(int x1, int y1, int x2, int y2)
	{
		p1.x = x1;
		p1.y = y1;
		p2.x = x2;
		p2.y = y2;
	}

	inline INT2 Random() const
	{
		return INT2(random(p1.x, p2.x), random(p1.y, p2.y));
	}

	inline IBOX2D LeftBottomPart() const
	{
		return IBOX2D(p1.x, p1.y, p1.x + (p2.x - p1.x) / 2, p1.y + (p2.y - p1.y) / 2);
	}
	inline IBOX2D RightBottomPart() const
	{
		return IBOX2D(p1.x + (p2.x - p1.x) / 2, p1.y, p2.x, p1.y + (p2.y - p1.y) / 2);
	}
	inline IBOX2D LeftTopPart() const
	{
		return IBOX2D(p1.x, p1.y + (p2.y - p1.y) / 2, p1.x + (p2.x - p1.x) / 2, p2.y);
	}
	inline IBOX2D RightTopPart() const
	{
		return IBOX2D(p1.x + (p2.x - p1.x) / 2, p1.y + (p2.y - p1.y) / 2, p2.x, p2.y);
	}
};

//-----------------------------------------------------------------------------
// Prostok�t na float
//-----------------------------------------------------------------------------
struct BOX2D
{
	VEC2 v1, v2;

	BOX2D() {}
	BOX2D(float _minx, float _miny, float _maxx, float _maxy) : v1(_minx, _miny), v2(_maxx, _maxy)
	{
		assert(v1.x <= v2.x && v1.y <= v2.y);
	}
	BOX2D(const VEC2& _v1, const VEC2& _v2) : v1(_v1), v2(_v2)
	{
		assert(v1.x <= v2.x && v1.y <= v2.y);
	}
	BOX2D(const BOX2D& _box) : v1(_box.v1), v2(_box.v2)
	{
		// assert fires on resize
		//assert(v1.x <= v2.x && v1.y <= v2.y);
	}
	BOX2D(float _x, float _y) : v1(_x, _y), v2(_x, _y)
	{
	}
	explicit BOX2D(const VEC2& v) : v1(v), v2(v)
	{
	}
	BOX2D(const BOX2D& _box, float margin) : v1(_box.v1.x - margin, _box.v1.y - margin), v2(_box.v2.x + margin, _box.v2.y + margin)
	{
	}

	inline VEC2 Midpoint() const
	{
		return v1 + (v2 - v1) / 2;
	}

	inline bool IsInside(const VEC3& pos) const
	{
		return pos.x >= v1.x && pos.x <= v2.x && pos.z >= v1.y && pos.z <= v2.y;
	}

	inline float SizeX() const { return abs(v2.x - v1.x); }
	inline float SizeY() const { return abs(v2.y - v1.y); }
	inline VEC2 Size() const { return VEC2(SizeX(), SizeY()); }

	inline bool IsValid() const
	{
		return v1.x <= v2.x && v1.y <= v2.y;
	}

	inline VEC2 GetRandomPos() const
	{
		return VEC2(random(v1.x, v2.x), random(v1.y, v2.y));
	}
	inline VEC3 GetRandomPos3(float y = 0.f) const
	{
		return VEC3(random(v1.x, v2.x), y, random(v1.y, v2.y));
	}

	inline VEC2 LeftTop() const
	{
		return v1;
	}
	inline VEC2 RightBottom() const
	{
		return v2;
	}
	inline VEC2 RightTop() const
	{
		return VEC2(v2.x, v1.y);
	}
	inline VEC2 LeftBottom() const
	{
		return VEC2(v1.x, v2.y);
	}

	inline BOX2D LeftBottomPart() const
	{
		return BOX2D(v1.x, v1.y, v1.x + (v2.x - v1.x) / 2, v1.y + (v2.y - v1.y) / 2);
	}
	inline BOX2D RightBottomPart() const
	{
		return BOX2D(v1.x + (v2.x - v1.x) / 2, v1.y, v2.x, v1.y + (v2.y - v1.y) / 2);
	}
	inline BOX2D LeftTopPart() const
	{
		return BOX2D(v1.x, v1.y + (v2.y - v1.y) / 2, v1.x + (v2.x - v1.x) / 2, v2.y);
	}
	inline BOX2D RightTopPart() const
	{
		return BOX2D(v1.x + (v2.x - v1.x) / 2, v1.y + (v2.y - v1.y) / 2, v2.x, v2.y);
	}

	inline void ToRectangle(float& x, float& y, float& w, float& h) const
	{
		x = v1.x + (v2.x - v1.x) / 2;
		y = v1.y + (v2.y - v1.y) / 2;
		w = (v2.x - v1.x) / 2;
		h = (v2.y - v1.y) / 2;
	}
};

//-----------------------------------------------------------------------------
// Sze�cian opisany dwoma punktami
//-----------------------------------------------------------------------------
struct BOX
{
	VEC3 v1, v2;

	BOX() {}
	BOX(float _minx, float _miny, float _minz, float _maxx, float _maxy, float _maxz) : v1(_minx, _miny, _minz), v2(_maxx, _maxy, _maxz)
	{
		assert(v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z);
	}
	BOX(const VEC3& _v1, const VEC3& _v2) : v1(_v1), v2(_v2)
	{
		assert(v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z);
	}
	BOX(const BOX& _box) : v1(_box.v1), v2(_box.v2)
	{
		// assert fires on resize
		//assert(v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z);
	}
	BOX(float _x, float _y, float _z) : v1(_x, _y, _z), v2(_x, _y, _z)
	{
	}
	explicit BOX(const VEC3& v) : v1(v), v2(v)
	{
	}

	inline void Create(const VEC3& _v1, const VEC3& _v2)
	{
		MinMax(_v1, _v2, v1, v2);
	}

	inline VEC3 Midpoint() const
	{
		return v1 + (v2 - v1) / 2;
	}

	inline float SizeX() const { return abs(v2.x - v1.x); }
	inline float SizeY() const { return abs(v2.y - v1.y); }
	inline float SizeZ() const { return abs(v2.z - v1.z); }
	inline VEC3 Size() const { return VEC3(SizeX(), SizeY(), SizeZ()); }
	inline VEC2 SizeXZ() const { return VEC2(SizeX(), SizeZ()); }

	inline bool IsValid() const
	{
		return v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z;
	}

	inline bool IsInside(const VEC3& v) const
	{
		return v.x >= v1.x && v.x <= v2.x && v.y >= v1.y && v.y <= v2.y && v.z >= v1.z && v.z <= v2.z;
	}

	inline VEC3 GetRandomPos() const
	{
		return VEC3(random(v1.x, v2.x), random(v1.y, v2.y), random(v1.z, v2.z));
	}
};

//-----------------------------------------------------------------------------
// Matrix 4x4
//-----------------------------------------------------------------------------
struct MATRIX
{
	typedef Eigen::Matrix4f Type;

	union
	{
		Type ev;
		struct
		{
			float _11, _12, _13, _14,
				_21, _22, _23, _24,
				_31, _32, _33, _34,
				_41, _42, _43, _44;
		};
		float array[4][4];
	};

	inline MATRIX() {}
	inline MATRIX(const MATRIX& m) : ev(m.ev) {}
	inline explicit MATRIX(const Type& ev) : ev(ev) {}

	inline MATRIX& operator = (const MATRIX& m)
	{
		ev = m.ev;
		return *this;
	}

	inline operator float* ()
	{
		return &array[0][0];
	}

	inline operator const float* () const
	{
		return &array[0][0];
	}

	inline bool operator == (const MATRIX& m) const
	{
		return ev == m.ev;
	}

	inline bool operator != (const MATRIX& m) const
	{
		return ev != m.ev;
	}

	inline float& operator () (uint row, uint col)
	{
		assert(row <= 3 && col <= 3);
		return array[row][col];
	}

	inline float operator () (uint row, uint col) const
	{
		assert(row <= 3 && col <= 3);
		return array[row][col];
	}

	inline MATRIX inverse() const
	{
		return MATRIX(ev.inverse());
	}

	inline void inverse_me()
	{
		ev = ev.inverse();
	}

	inline VEC3 transform_coord(const VEC3& v) const
	{
		auto result = ev * VEC4::Type(v.x, v.y, v.z, 1.f);
		float w = result.w();
		return VEC3(result.x() / w, result.y() / w, result.z() / w);
	}
};

//-----------------------------------------------------------------------------
void CreateAABBOX(BOX& out_box, const MATRIX& mat);

//-----------------------------------------------------------------------------
// obr�cony bounding box
//-----------------------------------------------------------------------------
struct OOBBOX
{
	VEC3 pos;
	VEC3 size;
	MATRIX rot;
};

// inna wersja, okarze si� czy lepszy algorymt
struct OOB
{
	VEC3 c; // �rodek
	VEC3 u[3]; // obr�t po X,Y,Z
	VEC3 e; // po�owa rozmiaru
};

//-----------------------------------------------------------------------------
// Inline functions that required forward declaration
inline INT2::INT2(const VEC2& v) : x(int(v.x)), y(int(v.y)) {}
inline INT2::INT2(const VEC3& v) : x(int(v.x)), y(int(v.z)) {}

struct PLANE
{
	typedef Eigen::Hyperplane<float, 3> Type;

	union
	{
		struct
		{
			float a;
			float b;
			float c;
			float d;
		};
		Type ev;
	};

	inline PLANE() {}
	inline PLANE(float a, float b, float c, float d) : a(a), b(b), c(c), d(d) {}
	inline PLANE(const PLANE& p) : ev(p.ev) {}
	inline explicit PLANE(const Type& ev) : ev(ev) {}
	inline ~PLANE() {}

	inline PLANE& operator = (const PLANE& p)
	{
		ev = p.ev;
		return *this;
	}

	inline float dot(const VEC3& v) const
	{
		return a*v.x + b*v.y + c*v.z + d;
	}

	inline float dot(const VEC4& v) const
	{
		return a*v.x + b*v.y + c*v.z + d*v.w;
	}

	inline void normalize_me()
	{
		ev.normalize();
	}
};

static_assert(sizeof(PLANE) == sizeof(float) * 4, "PLANE size mismatch.");

//-----------------------------------------------------------------------------
// KOLIZJE
//-----------------------------------------------------------------------------
// promie� - AABOX
bool RayToBox(const VEC3 &RayOrig, const VEC3 &RayDir, const BOX &Box, float *OutT);
// promie� - p�aszczyzna
bool RayToPlane(const VEC3 &RayOrig, const VEC3 &RayDir, const PLANE& Plane, float *OutT);
// promie� - sfera
bool RayToSphere(const VEC3& ray_pos, const VEC3& ray_dir, const VEC3& center, float radius, float& dist);
// promie� - tr�jk�t
bool RayToTriangle(const VEC3& ray_pos, const VEC3& ray_dir, const VEC3& v1, const VEC3& v2, const VEC3& v3, float& dist);
// prostok�t - prostok�t
bool RectangleToRectangle(float x1, float y1, float x2, float y2, float a1, float b1, float a2, float b2);
// okr�g - prostok�t
bool CircleToRectangle(float circlex, float circley, float radius, float rectx, float recty, float w, float h);
// odcinek - odcinek (2d)
bool LineToLine(const VEC2& start1, const VEC2& end1, const VEC2& start2, const VEC2& end2, float* t = nullptr);
// odcinek - prostok�t
bool LineToRectangle(const VEC2& start, const VEC2& end, const VEC2& rect_pos, const VEC2& rect_pos2, float* t = nullptr);
inline bool LineToRectangle(const VEC3& start, const VEC3& end, const VEC2& rect_pos, const VEC2& rect_pos2, float* t = nullptr)
{
	return LineToRectangle(VEC2(start.x, start.z), VEC2(end.x, end.z), rect_pos, rect_pos2, t);
}
inline bool LineToRectangleSize(const VEC2& start, const VEC2& end, const VEC2& rect_pos, const VEC2& rect_size, float* t = nullptr)
{
	return LineToRectangle(start, end, rect_pos - rect_size, rect_pos + rect_size, t);
}
// sze�cian - sze�cian
bool BoxToBox(const BOX& box1, const BOX& box2);
// obr�cony sze�cian - obr�cony sze�cian
// punkt kontaktu jest opcjonalny (jest to u�redniony wynik z maksymalnie 4 kontakt�w)
bool OrientedBoxToOrientedBox(const OOBBOX& obox1, const OOBBOX& obox2, VEC3* contact);
// kolizja ko�o - obr�cony prostok�t
bool CircleToRotatedRectangle(float cx, float cy, float radius, float rx, float ry, float w, float h, float rot);
// kolizja dw�ch obr�conych prostok�t�w (mo�na by zrobi� optymalizacje �e jeden tylko jest obr�cony ale nie wiem jak :3)
struct RotRect
{
	VEC2 center, size;
	float rot;
};
bool RotatedRectanglesCollision(const RotRect& r1, const RotRect& r2);
inline bool CircleToCircle(float cx1, float cy1, float r1, float cx2, float cy2, float r2)
{
	float r = (r1 + r2);
	return distance_sqrt(cx1, cy1, cx2, cy2) < r * r;
}
bool SphereToBox(const VEC3& pos, float radius, const BOX& box);
// kolizja promienia (A->B) z cylindrem (P->Q, promie� R)
int RayToCylinder(const VEC3& ray_A, const VEC3& ray_B, const VEC3& cylinder_P, const VEC3& cylinder_Q, float radius, float& t);
// kolizja OOB z OOB
bool OOBToOOB(const OOB& a, const OOB& b);
// odleg�o�� punktu od prostok�ta
float DistanceRectangleToPoint(const VEC2& pos, const VEC2& size, const VEC2& pt);
// x0,y0 - point
float PointLineDistance(float x0, float y0, float x1, float y1, float x2, float y2);
float GetClosestPointOnLineSegment(const VEC2& A, const VEC2& B, const VEC2& P, VEC2& result);


//-----------------------------------------------------------------------------
// struktura do sprawdzania czy obiekt jest widoczny na ekranie
// kod z TFQE
//-----------------------------------------------------------------------------
struct FrustumPlanes
{
	PLANE Planes[6];

	FrustumPlanes() {}
	explicit FrustumPlanes(const MATRIX &WorldViewProj) { Set(WorldViewProj); }
	void Set(const MATRIX &WorldViewProj);

	// zwraca punkty na kraw�dziach frustuma
	void GetPoints(VEC3* points) const;
	static void GetPoints(const MATRIX& WorldViewProj, VEC3* points);
	/*void GetFrustumBox(BOX& box) const;
	static void GetFrustumBox(const MATRIX& WorldViewProj,BOX& box);
	void GetMidpoint(VEC3& midpoint) const;*/

	// funkcje sprawdzaj�ce
	// Zwraca true, je�li punkt nale�y do wn�trza podanego frustuma
	bool PointInFrustum(const VEC3 &p) const;

	// Zwraca true, je�li podany prostopad�o�cian jest widoczny cho� troch� w bryle widzenia
	// Uwaga! W rzadkich przypadkach mo�e stwierdzi� kolizj� chocia� jej nie ma.
	bool BoxToFrustum(const BOX &Box) const;
	// 0 - poza, 1 - poza zasi�giem, 2 - w zasi�gu
	int BoxToFrustum2(const BOX& box) const;

	bool BoxToFrustum(const BOX2D& box) const;

	// Zwraca true, je�li AABB jest w ca�o�ci wewn�trz frustuma
	bool BoxInFrustum(const BOX &Box) const;

	// Zwraca true, je�li sfera koliduje z frustumem
	// Uwaga! W rzadkich przypadkach mo�e stwierdzi� kolizj� chocia� jej nie ma.
	bool SphereToFrustum(const VEC3 &SphereCenter, float SphereRadius) const;

	// Zwraca true, je�li sfera zawiera si� w ca�o�ci wewn�trz frustuma
	bool SphereInFrustum(const VEC3 &SphereCenter, float SphereRadius) const;
};

// check for overflow a + b, and return value
inline bool checked_add(uint a, uint b, uint& result)
{
	uint64 r = (uint64)a + b;
	if(r > std::numeric_limits<uint>::max())
		return false;
	result = (uint)r;
	return true;
}

// check for overflow a * b + c, and return value
inline bool checked_multiply_add(uint a, uint b, uint c, uint& result)
{
	uint64 r = (uint64)a * b + c;
	if(r > std::numeric_limits<uint>::max())
		return false;
	result = (uint)r;
	return true;
}

extern const VEC2 POISSON_DISC_2D[];
extern const int poisson_disc_count;

//-----------------------------------------------------------------------------
inline void Split3(int val, int& a, int& b, int& c)
{
	a = (val & 0xFF);
	b = ((val & 0xFF00) >> 8);
	c = ((val & 0xFF0000) >> 16);
}

inline int Join3(int a, int b, int c)
{
	return (a & 0xFF) | ((b & 0xFF) << 8) | ((c & 0xFF) << 16);
}

//-----------------------------------------------------------------------------
struct Pixel
{
	INT2 pt;
	byte alpha;

	Pixel(int x, int y, byte alpha = 0) : pt(x, y), alpha(alpha) {}
};

void PlotLine(int x0, int y0, int x1, int y1, float th, vector<Pixel>& pixels);
void PlotQuadBezierSeg(int x0, int y0, int x1, int y1, int x2, int y2, float w, float th, vector<Pixel>& pixels);
void PlotQuadBezier(int x0, int y0, int x1, int y1, int x2, int y2, float w, float th, vector<Pixel>& pixels);
void PlotCubicBezierSeg(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, float th, vector<Pixel>& pixels);
void PlotCubicBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, float th, vector<Pixel>& pixels);

//-----------------------------------------------------------------------------
inline void ColorToVec(DWORD c, VEC4& v)
{
	v.x = float((c & 0xFF0000) >> 16) / 255;
	v.y = float((c & 0xFF00) >> 8) / 255;
	v.z = float(c & 0xFF) / 255;
	v.w = float((c & 0xFF000000) >> 24) / 255;
}
