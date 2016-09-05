#include "Pch.h"
#include "Base.h"
#include "Math.h"

void test()
{
	MATRIX m;
	VEC3 v(1, 2, 3);
	VEC4 v4(v, 1);
	VEC4 vr(m.ev * v4.ev);
	//VEC4 vr2(v4.ev * m.ev);
	VEC3 r(vr.x / vr.w, vr.y / vr.w, vr.z / vr.w);
}
