#include "Pch.h"
#include "Core/Base.h"
#include "Core/Math.h"

RNG _RNG;

//=================================================================================================
// Zwraca k�t pomi�dzy dwoma punktami
//=================================================================================================
float angle(float x1, float y1, float x2, float y2)
{
	float x = x2 - x1;
	float y = y2 - y1;

	if(x == 0)
	{
		if(y == 0)
			return 0;
		else if(y > 0)
			return PI / 2.f;
		else
			return PI * 3.f / 2.f;
	}
	else if(y == 0)
	{
		if(x > 0)
			return 0;
		else
			return PI;
	}
	else
	{
		if(x < 0)
			return atan(y / x) + PI;
		else if(y < 0)
			return atan(y / x) + (2 * PI);
		else
			return atan(y / x);
	}
}

//=================================================================================================
// W kt�r� stron� trzeba si� obr�ci� �eby by�o najszybciej
//=================================================================================================
float shortestArc(float a, float b)
{
	if(fabs(b - a) < PI)
		return b - a;
	if(b>a)
		return b - a - PI*2.0f;
	return b - a + PI*2.0f;
}

//=================================================================================================
// Interpolacja k�t�w
//=================================================================================================
void lerp_angle(float& angle, float from, float to, float t)
{
	if(to > angle)
	{
		while(to - angle > PI)
			to -= PI * 2;
	}
	else
	{
		while(to - angle < -PI)
			to += PI * 2;
	}

	angle = from + t * (to - from);
}

//=================================================================================================
// Kolizja promienia z prostopad�o�cianem
// Je�li promie� nie przecina prostopad�o�cianu, zwraca false.
// Je�li promie� przecina prostopad�o�cian, zwraca true i przez OutT zwraca odleg�o�� w wielokrotno�ciach d�ugo�ci RayDir.
// Je�li promie� przecina prostopad�o�cian od ty�u, funkcja te� zwraca true i zwraca OutT ujemne.
// Je�li RayOrig jest wewn�trz prostopad�o�cianu, funkcja zwraca true i OutT = 0.
// funkcja z TFQE
//=================================================================================================
bool RayToBox(const VEC3 &RayOrig, const VEC3 &RayDir, const BOX &Box, float *OutT)
{
	// removed xn, yn, zn
	bool inside = true;
	float xt;//, xn;

	if(RayOrig.x < Box.v1.x)
	{
		xt = Box.v1.x - RayOrig.x;
		xt /= RayDir.x;
		//xn = -1.0f;
		inside = false;
	}
	else if(RayOrig.x > Box.v2.x)
	{
		xt = Box.v2.x - RayOrig.x;
		xt /= RayDir.x;
		//xn = 1.0f;
		inside = false;
	}
	else
		xt = -1.0f;

	float yt;//, yn;

	if(RayOrig.y < Box.v1.y)
	{
		yt = Box.v1.y - RayOrig.y;
		yt /= RayDir.y;
		//yn = -1.0f;
		inside = false;
	}
	else if(RayOrig.y > Box.v2.y)
	{
		yt = Box.v2.y - RayOrig.y;
		yt /= RayDir.y;
		//yn = 1.0f;
		inside = false;
	}
	else
		yt = -1.0f;

	float zt;//, zn;

	if(RayOrig.z < Box.v1.z)
	{
		zt = Box.v1.z - RayOrig.z;
		zt /= RayDir.z;
		//zn = -1.0f;
		inside = false;
	}
	else if(RayOrig.z > Box.v2.z)
	{
		zt = Box.v2.z - RayOrig.z;
		zt /= RayDir.z;
		//zn = 1.0f;
		inside = false;
	}
	else
		zt = -1.0f;

	if(inside)
	{
		*OutT = 0.0f;
		return true;
	}

	// Select the farthest plane - this is the plane of intersection
	int plane = 0;

	float t = xt;
	if(yt > t)
	{
		plane = 1;
		t = yt;
	}

	if(zt > t)
	{
		plane = 2;
		t = zt;
	}

	// Check if the point of intersection lays within the box face

	switch(plane)
	{
	case 0: // ray intersects with yz plane
		{
			float y = RayOrig.y + RayDir.y * t;
			if(y < Box.v1.y || y > Box.v2.y) return false;
			float z = RayOrig.z + RayDir.z * t;
			if(z < Box.v1.z || z > Box.v2.z) return false;
		}
		break;
	case 1: // ray intersects with xz plane
		{
			float x = RayOrig.x + RayDir.x * t;
			if(x < Box.v1.x || x > Box.v2.x) return false;
			float z = RayOrig.z + RayDir.z * t;
			if(z < Box.v1.z || z > Box.v2.z) return false;
		}
		break;
	default:
	case 2: // ray intersects with xy plane
		{
			float x = RayOrig.x + RayDir.x * t;
			if(x < Box.v1.x || x > Box.v2.x) return false;
			float y = RayOrig.y + RayDir.y * t;
			if(y < Box.v1.y || y > Box.v2.y) return false;
		}
		break;
	}

	*OutT = t;
	return true;
}

bool CircleToRectangle(float circlex, float circley, float radius, float rectx, float recty, float w, float h)
{
	//
	//        /|\ -h
	//         |
	//         |
	//  -w <--(x,y)--> w
	//         |
	//         |
	//        \|/  h
	float dist_x = abs(circlex - rectx);
	float dist_y = abs(circley - recty);

	if((dist_x > (w + radius)) || (dist_y > (h + radius)))
		return false;

	if((dist_x <= w) || (dist_y <= h))
		return true;

	float dx = dist_x - w;
	float dy = dist_y - h;

	return (dx*dx + dy*dy) <= (radius*radius);
}

void FrustumPlanes::Set(const MATRIX &WorldViewProj)
{
	// Left clipping plane
	Planes[0].a = WorldViewProj._14 + WorldViewProj._11;
	Planes[0].b = WorldViewProj._24 + WorldViewProj._21;
	Planes[0].c = WorldViewProj._34 + WorldViewProj._31;
	Planes[0].d = WorldViewProj._44 + WorldViewProj._41;

	// Right clipping plane
	Planes[1].a = WorldViewProj._14 - WorldViewProj._11;
	Planes[1].b = WorldViewProj._24 - WorldViewProj._21;
	Planes[1].c = WorldViewProj._34 - WorldViewProj._31;
	Planes[1].d = WorldViewProj._44 - WorldViewProj._41;

	// Top clipping plane
	Planes[2].a = WorldViewProj._14 - WorldViewProj._12;
	Planes[2].b = WorldViewProj._24 - WorldViewProj._22;
	Planes[2].c = WorldViewProj._34 - WorldViewProj._32;
	Planes[2].d = WorldViewProj._44 - WorldViewProj._42;

	// Bottom clipping plane
	Planes[3].a = WorldViewProj._14 + WorldViewProj._12;
	Planes[3].b = WorldViewProj._24 + WorldViewProj._22;
	Planes[3].c = WorldViewProj._34 + WorldViewProj._32;
	Planes[3].d = WorldViewProj._44 + WorldViewProj._42;

	// Near clipping plane
	Planes[4].a = WorldViewProj._13;
	Planes[4].b = WorldViewProj._23;
	Planes[4].c = WorldViewProj._33;
	Planes[4].d = WorldViewProj._43;

	// Far clipping plane
	Planes[5].a = WorldViewProj._14 - WorldViewProj._13;
	Planes[5].b = WorldViewProj._24 - WorldViewProj._23;
	Planes[5].c = WorldViewProj._34 - WorldViewProj._33;
	Planes[5].d = WorldViewProj._44 - WorldViewProj._43;

	for(int i = 0; i < 6; ++i)
		Planes[i].Normalized();
}

// Uniwersalny, brakuj�cy w C++ operator dos�ownego rzutowania (reintepretacji)
template <typename destT, typename srcT>
destT &absolute_cast(srcT &v)
{
	return reinterpret_cast<destT&>(v);
}
template <typename destT, typename srcT>
const destT &absolute_cast(const srcT &v)
{
	return reinterpret_cast<const destT&>(v);
}

#define FLOAT_ALMOST_ZERO(F) ((absolute_cast<unsigned>(F) & 0x7f800000L) == 0)

bool Intersect3Planes(const PLANE &P1, const PLANE &P2, const PLANE &P3, VEC3 *OutP)
{
	// Inny algorytm (albo inaczej zapisany) jest te� w ksi��ce "3D Math Primer for Graphics and Game Development", str. 286.
	float fDet;
	float MN[9] = { P1.a, P1.b, P1.c, P2.a, P2.b, P2.c, P3.a, P3.b, P3.c };
	float IMN[9] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	float MD[3] = { -P1.d, -P2.d , -P3.d };

	IMN[0] = MN[4] * MN[8] - MN[5] * MN[7];
	IMN[3] = -(MN[3] * MN[8] - MN[5] * MN[6]);
	IMN[6] = MN[3] * MN[7] - MN[4] * MN[6];

	fDet = MN[0] * IMN[0] + MN[1] * IMN[3] + MN[2] * IMN[6];

	if(FLOAT_ALMOST_ZERO(fDet))
		return false;

	IMN[1] = -(MN[1] * MN[8] - MN[2] * MN[7]);
	IMN[4] = MN[0] * MN[8] - MN[2] * MN[6];
	IMN[7] = -(MN[0] * MN[7] - MN[1] * MN[6]);
	IMN[2] = MN[1] * MN[5] - MN[2] * MN[4];
	IMN[5] = -(MN[0] * MN[5] - MN[2] * MN[3]);
	IMN[8] = MN[0] * MN[4] - MN[1] * MN[3];

	fDet = 1.0f / fDet;

	IMN[0] *= fDet;
	IMN[1] *= fDet;
	IMN[2] *= fDet;
	IMN[3] *= fDet;
	IMN[4] *= fDet;
	IMN[5] *= fDet;
	IMN[6] *= fDet;
	IMN[7] *= fDet;
	IMN[8] *= fDet;

	OutP->x = IMN[0] * MD[0] + IMN[1] * MD[1] + IMN[2] * MD[2];
	OutP->y = IMN[3] * MD[0] + IMN[4] * MD[1] + IMN[5] * MD[2];
	OutP->z = IMN[6] * MD[0] + IMN[7] * MD[1] + IMN[8] * MD[2];

	return true;
}

void FrustumPlanes::GetPoints(VEC3* points) const
{
	assert(points);

	Intersect3Planes(Planes[4], Planes[0], Planes[3], &points[0]);
	Intersect3Planes(Planes[4], Planes[1], Planes[3], &points[1]);
	Intersect3Planes(Planes[4], Planes[0], Planes[2], &points[2]);
	Intersect3Planes(Planes[4], Planes[1], Planes[2], &points[3]);
	Intersect3Planes(Planes[5], Planes[0], Planes[3], &points[4]);
	Intersect3Planes(Planes[5], Planes[1], Planes[3], &points[5]);
	Intersect3Planes(Planes[5], Planes[0], Planes[2], &points[6]);
	Intersect3Planes(Planes[5], Planes[1], Planes[2], &points[7]);
}

void FrustumPlanes::GetPoints(const MATRIX& WorldViewProj, VEC3* points)
{
	assert(points);

	MATRIX WorldViewProjInv = WorldViewProj.Inverse();

	VEC3 P[] = {
		VEC3(-1.f, -1.f, 0.f), VEC3(+1.f, -1.f, 0.f),
		VEC3(-1.f, +1.f, 0.f), VEC3(+1.f, +1.f, 0.f),
		VEC3(-1.f, -1.f, 1.f), VEC3(+1.f, -1.f, 1.f),
		VEC3(-1.f, +1.f, 1.f), VEC3(+1.f, +1.f, 1.f) };

	for(int i = 0; i < 8; ++i)
		points[0] = WorldViewProjInv.TransformCoord(P[i]);
}

/*void FrustumPlanes::GetFrustumBox(BOX& box) const
{
VEC3 points[8];
GetPoints(points);
math::MinMax(points,8,&box.v1,&box.v2);
}

void FrustumPlanes::GetFrustumBox(const MATRIX& WorldViewProj,BOX& box)
{
VEC3 points[8];
GetPoints(WorldViewProj,points);
math::MinMax(points,8,&box.v1,&box.v2);
}

void FrustumPlanes::GetMidpoint(VEC3& midpoint) const
{
VEC3 points[8];
GetPoints(points);
math::Midpoint(midpoint, points, 8);
}*/

bool FrustumPlanes::PointInFrustum(const VEC3 &p) const
{
	for(int i = 0; i < 6; ++i)
	{
		if(Planes[i].Dot(p) <= 0.f)
			return false;
	}

	return true;
}

bool FrustumPlanes::BoxToFrustum(const BOX &Box) const
{
	// Na podstawie ksi��ki "3D Game Engine Programming", Stefan Zerbst with Oliver Duvel
	VEC3 vmin;

	for(int i = 0; i < 6; i++)
	{
		if(Planes[i].a <= 0.0f)
			vmin.x = Box.v1.x;
		else
			vmin.x = Box.v2.x;

		if(Planes[i].b <= 0.0f)
			vmin.y = Box.v1.y;
		else
			vmin.y = Box.v2.y;

		if(Planes[i].c <= 0.0f)
			vmin.z = Box.v1.z;
		else
			vmin.z = Box.v2.z;

		if(Planes[i].Dot(vmin) < 0.0f)
			return false;
	}

	return true;
}

int FrustumPlanes::BoxToFrustum2(const BOX &Box) const
{
	VEC3 vmin;

	for(int i = 0; i < 6; i++)
	{
		if(Planes[i].a <= 0.0f)
			vmin.x = Box.v1.x;
		else
			vmin.x = Box.v2.x;

		if(Planes[i].b <= 0.0f)
			vmin.y = Box.v1.y;
		else
			vmin.y = Box.v2.y;

		if(Planes[i].c <= 0.0f)
			vmin.z = Box.v1.z;
		else
			vmin.z = Box.v2.z;

		if(Planes[i].Dot(vmin) < 0.0f)
		{
			if(i == 5)
				return 1;
			else
				return 0;
		}
	}

	return 2;
}

bool FrustumPlanes::BoxToFrustum(const BOX2D& box) const
{
	VEC3 vmin;

	for(int i = 0; i < 6; i++)
	{
		if(Planes[i].a <= 0.0f)
			vmin.x = box.v1.x;
		else
			vmin.x = box.v2.x;

		if(Planes[i].b <= 0.0f)
			vmin.y = 0.f;
		else
			vmin.y = 25.f;

		if(Planes[i].c <= 0.0f)
			vmin.z = box.v1.y;
		else
			vmin.z = box.v2.y;

		if(Planes[i].Dot(vmin) < 0.0f)
			return false;
	}

	return true;
}

bool FrustumPlanes::BoxInFrustum(const BOX &Box) const
{
	// Ka�dy punkt AABB musi le�e� w jego wn�trzu
	if(!PointInFrustum(Box.v1)) return false;
	if(!PointInFrustum(Box.v2)) return false;
	if(!PointInFrustum(VEC3(Box.v2.x, Box.v1.y, Box.v1.z))) return false;
	if(!PointInFrustum(VEC3(Box.v1.x, Box.v2.y, Box.v1.z))) return false;
	if(!PointInFrustum(VEC3(Box.v2.x, Box.v2.y, Box.v1.z))) return false;
	if(!PointInFrustum(VEC3(Box.v1.x, Box.v1.y, Box.v2.z))) return false;
	if(!PointInFrustum(VEC3(Box.v2.x, Box.v1.y, Box.v2.z))) return false;
	if(!PointInFrustum(VEC3(Box.v1.x, Box.v2.y, Box.v2.z))) return false;

	return true;
}

bool FrustumPlanes::SphereToFrustum(const VEC3 &SphereCenter, float SphereRadius) const
{
	SphereRadius = -SphereRadius;

	for(int i = 0; i < 6; ++i)
	{
		if(Planes[i].Dot(SphereCenter) <= SphereRadius)
			return false;
	}

	return true;
}

bool FrustumPlanes::SphereInFrustum(const VEC3 &SphereCenter, float SphereRadius) const
{
	for(int i = 0; i < 6; ++i)
	{
		if(Planes[0].Dot(SphereCenter) < SphereRadius)
			return false;
	}

	return true;
}

// funkcja z TFQE
bool RayToPlane(const VEC3 &RayOrig, const VEC3 &RayDir, const PLANE &Plane, float *OutT)
{
	// Napisane na podstawie:
	// http://www.siggraph.org/education/materials/HyperGraph/raytrace/rayplane_intersection.htm

	//       A xo + B yo + C zo + D
	// t = - ----------------------
	//         A xd + B yd + C zd

	// Ten sam wz�r jest w ksi��ce "3D Math Primer for Graphics and Game Development", str. 284.
	// Inna wersja dost�pna jest w ksi��ce: "3D Game Engine Programming", Stefan Zerbst with Oliver Duvel, str. 136.

	float VD = Plane.a * RayDir.x + Plane.b * RayDir.y + Plane.c * RayDir.z;
	if(VD == 0.0f)
		return false;

	*OutT = -(Plane.a * RayOrig.x + Plane.b * RayOrig.y + Plane.c * RayOrig.z + Plane.d) / VD;

	return true;
}

// funkcja z TFQE
bool RayToSphere(const VEC3& _ray_pos, const VEC3& _ray_dir, const VEC3& _center, float _radius, float& _dist)
{
	// R�wnanie kwadratowe.
	// Napisane samodzielnie z ma�� pomoc�:
	// http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter1.htm
	// link znaleziony na:
	// http://www.realtimerendering.com/int/
	VEC3 RayOrig_minus_SphereCenter = _ray_pos - _center;
	float a = _ray_dir.Dot(_ray_dir); // ?
	float b = 2.f * _ray_dir.Dot(RayOrig_minus_SphereCenter);
	float c = RayOrig_minus_SphereCenter.Dot(RayOrig_minus_SphereCenter) - (_radius * _radius);
	float Delta = b * b - 4.f * a * c;

	if(Delta < 0.f)
		return false;

	float a_2 = 2.f * a;
	float minus_b = -b;
	float sqrt_Delta = sqrtf(Delta);

	// Pierwszy pierwiastek - ten mniejszy
	_dist = (minus_b - sqrt_Delta) / a_2;
	// Przypadek �e ca�a sfera jest przed RayOrig - pierwiastek mniejszy to wynik
	if(_dist >= 0.f)
		return true;
	// Drugi pierwiastek - ten wi�kszy
	_dist = (minus_b + sqrt_Delta) / a_2;
	// Przypadek �e poczatek promienia jest wewn�trz sfery
	if(_dist >= 0.f)
	{
		_dist = 0.f;
		return true;
	}
	// Przypadek �e sfera jest z ty�u za promieniem
	return false;
}

// funkcja z TFQE
// kod do backface cullingu zakomentowany, lepiej da� to jako osobn� funkcj�
// zwraca ujemn� warto�� je�li przecina promie� od ty�u
bool RayToTriangle(const VEC3& _ray_pos, const VEC3& _ray_dir, const VEC3& _v1, const VEC3& _v2, const VEC3& _v3, float& _dist)
{
	VEC3 tvec, pvec, qvec;

	// find vectors for two edges sharing vert0
	VEC3 edge1 = _v2 - _v1;
	VEC3 edge2 = _v3 - _v1;

	// begin calculating determinant - also used to calculate U parameter
	pvec = _ray_dir.Cross(edge2);

	// if determinant is near zero, ray lies in plane of triangle
	float det = edge1.Dot(pvec);
	//if (BackfaceCulling && det < 0.0f)
	//	return false;
	if(FLOAT_ALMOST_ZERO(det))
		return false;
	float inv_det = 1.0f / det;

	// calculate distance from vert0 to ray origin
	tvec = _ray_pos - _v1;

	// calculate U parameter and test bounds
	float u = tvec.Dot(pvec) * inv_det;
	if(u < 0.0f || u > 1.0f)
		return false;

	// prepare to test V parameter
	qvec = tvec.Cross(edge1);

	// calculate V parameter and test bounds
	float v = _ray_dir.Dot(qvec) * inv_det;
	if(v < 0.0f || u + v > 1.0f)
		return false;

	// calculate t, ray intersects triangle
	_dist = edge2.Dot(qvec) * inv_det;
	return true;
}

bool LineToLine(const VEC2& start1, const VEC2& end1, const VEC2& start2, const VEC2& end2, float* t)
{
	float ua_t = (end2.x - start2.x)*(start1.y - start2.y) - (end2.y - start2.y)*(start1.x - start2.x);
	float ub_t = (end1.x - start1.x)*(start1.y - start2.y) - (end1.y - start1.y)*(start1.x - start2.x);
	float u_b = (end2.y - start2.y)*(end1.x - start1.x) - (end2.x - start2.x)*(end1.y - start1.y);

	if(u_b != 0)
	{
		float ua = ua_t / u_b;
		float ub = ub_t / u_b;
		if(0 <= ua&&ua <= 1 && 0 <= ub&&ub <= 1)
		{
			// przeci�cie
			if(t)
				*t = ua;
			return true;
		}
		else
		{
			// brak przeci�cia
			return false;
		}
	}
	else
	{
		if(ua_t == 0 || ub_t == 0)
		{
			// zbierzne
			if(t)
				*t = 0;
			return true;
		}
		else
		{
			// r�wnoleg�e
			return false;
		}
	}
}

bool LineToRectangle(const VEC2& start, const VEC2& end, const VEC2& rect_pos, const VEC2& rect_pos2, float* _t)
{
	assert(rect_pos.x <= rect_pos2.x && rect_pos.y <= rect_pos2.y);

	const VEC2 topRight(rect_pos2.x, rect_pos.y),
		bottomLeft(rect_pos.x, rect_pos2.y);

	if(_t)
	{
		float tt, t = 1.001f;

		if(LineToLine(start, end, rect_pos, topRight, &tt) && tt < t) t = tt;
		if(LineToLine(start, end, topRight, rect_pos2, &tt) && tt < t) t = tt;
		if(LineToLine(start, end, rect_pos2, bottomLeft, &tt) && tt < t) t = tt;
		if(LineToLine(start, end, bottomLeft, rect_pos, &tt) && tt < t) t = tt;

		*_t = t;

		return (t <= 1.f);
	}
	else
	{
		if(LineToLine(rect_pos, topRight, start, end))    return true;
		if(LineToLine(topRight, rect_pos2, start, end))   return true;
		if(LineToLine(rect_pos2, bottomLeft, start, end)) return true;
		if(LineToLine(bottomLeft, rect_pos, start, end))  return true;

		return false;
	}
}

void CreateAABBOX(BOX& out, const MATRIX& mat)
{
	VEC3 v1 = mat.TransformCoord(VEC3(-2, -2, -2));
	VEC3 v2 = mat.TransformCoord(VEC3(2, 2, 2));
	out.Create(v1, v2);
}

bool BoxToBox(const BOX& box1, const BOX& box2)
{
	return
		(box1.v1.x <= box2.v2.x) && (box1.v2.x >= box2.v1.x) &&
		(box1.v1.y <= box2.v2.y) && (box1.v2.y >= box2.v1.y) &&
		(box1.v1.z <= box2.v2.z) && (box1.v2.z >= box2.v1.z);
}

bool RectangleToRectangle(float x1, float y1, float x2, float y2, float a1, float b1, float a2, float b2)
{
	return (x1 <= a2) && (x2 >= a1) && (y1 <= b2) && (y2 >= b1);
}

inline float clamp2(float left, float val, float right)
{
	return clamp(val, left, right);
}

// podpierdolone z CommonLib Regedita
void ClosestPointInBox(VEC3 *Out, const BOX &Box, const VEC3 &p)
{
	Out->x = clamp2(Box.v1.x, p.x, Box.v2.x);
	Out->y = clamp2(Box.v1.y, p.y, Box.v2.y);
	Out->z = clamp2(Box.v1.z, p.z, Box.v2.z);
}

inline float LengthSq(const VEC3 &v)
{
	return v.x*v.x + v.y*v.y + v.z*v.z;
}

inline float DistanceSq(const VEC3 &p1, const VEC3 &p2)
{
	return LengthSq(p2 - p1);
}

bool SphereToBox(const VEC3 &SphereCenter, float SphereRadius, const BOX &Box)
{
	VEC3 PointInBox;
	ClosestPointInBox(&PointInBox, Box, SphereCenter);
	return DistanceSq(SphereCenter, PointInBox) < SphereRadius*SphereRadius;
}

/*
kwaterniony

float Quaternion::getPitch()
{
return atan2(2*(y*z + w*x), w*w - x*x - y*y + z*z);
}

float Quaternion::getYaw()
{
return asin(-2*(x*z - w*y));
}

float Quaternion::getRoll()
{
return atan2(2*(x*y + w*z), w*w + x*x - y*y - z*z);
}

heading = atan2(2*qy*qw-2*qx*qz , 1 - 2*qy2 - 2*qz2)
attitude = asin(2*qx*qy + 2*qz*qw)
bank = atan2(2*qx*qw-2*qy*qz , 1 - 2*qx2 - 2*qz2)

except when qx*qy + qz*qw = 0.5 (north pole)
which gives:
heading = 2 * atan2(x,w)
bank = 0
and when qx*qy + qz*qw = -0.5 (south pole)
which gives:
heading = -2 * atan2(x,w)
bank = 0
*/

// http://www.migapro.com/circle-and-rotated-rectangle-collision-detection/
bool CircleToRotatedRectangle(float cx, float cy, float radius, float rx, float ry, float w, float h, float rot)
{
	// Rotate circle's center point back
	/*float unrotatedCircleX = cos(rot) * (cx - rx) - sin(rot) * (cy - ry) + rx;
	float unrotatedCircleY = sin(rot) * (cx - rx) + cos(rot) * (cy - ry) + ry;

	// Closest point in the rectangle to the center of circle rotated backwards(unrotated)
	float closestX, closestY;

	// Find the unrotated closest x point from center of unrotated circle
	if (unrotatedCircleX  < rx)
	closestX = rx;
	else if (unrotatedCircleX  > rx + w)
	closestX = rx + w;
	else
	closestX = unrotatedCircleX;

	// Find the unrotated closest y point from center of unrotated circle
	if (unrotatedCircleY < ry)
	closestY = ry;
	else if (unrotatedCircleY > ry + h)
	closestY = ry + h;
	else
	closestY = unrotatedCircleY;

	// Determine collision
	return (distance_sqrt(unrotatedCircleX , unrotatedCircleY, closestX, closestY) < radius*radius);*/

	// doprowadzi�em ten algorytm do u�ywalno�ci
	const float //rot = _rot,
		sina = sin(rot),
		cosa = cos(rot),
		difx = cx - rx,
		dify = cy - ry,
		x = cosa * difx - sina * dify + rx,
		y = sina * difx + cosa * dify + ry;

	// ??? wcze�niej dzia�a�o jak zanegowa�em rot, teraz bez tego ???
	// mo�e co� jest jeszcze �le

	return CircleToRectangle(x, y, radius, rx, ry, w, h);
}

inline void RotateVector2DClockwise(VEC2& v, float ang)
{
	float t,
		cosa = cos(ang),
		sina = sin(ang);
	t = v.x;
	v.x = t*cosa + v.y*sina;
	v.y = -t*sina + v.y*cosa;
}

// Rotated Rectangles Collision Detection, Oren Becker, 2001
// http://ragestorm.net/samples/CDRR.C
bool RotatedRectanglesCollision(const RotRect& r1, const RotRect& r2)
{
	VEC2 A, B,   // vertices of the rotated rr2
		C,      // center of rr2
		BL, TR; // vertices of rr2 (bottom-left, top-right)

	float ang = r1.rot - r2.rot, // orientation of rotated rr1
		cosa = cos(ang),           // precalculated trigonometic -
		sina = sin(ang);           // - values for repeated use

	float t, x, a;      // temporary variables for various uses
	float dx;           // deltaX for linear equations
	float ext1, ext2;   // min/max vertical values

						// move r2 to make r1 cannonic
	C = r2.center - r1.center;

	// rotate r2 clockwise by r2.ang to make r2 axis-aligned
	RotateVector2DClockwise(C, r2.rot);

	// calculate vertices of (moved and axis-aligned := 'ma') r2
	BL = TR = C;
	BL -= r2.size;
	TR += r2.size;

	// calculate vertices of (rotated := 'r') r1
	A.x = -r1.size.y*sina; B.x = A.x; t = r1.size.x*cosa; A.x += t; B.x -= t;
	A.y = r1.size.y*cosa; B.y = A.y; t = r1.size.x*sina; A.y += t; B.y -= t;

	t = sina*cosa;

	// verify that A is vertical min/max, B is horizontal min/max
	if(t < 0)
	{
		t = A.x; A.x = B.x; B.x = t;
		t = A.y; A.y = B.y; B.y = t;
	}

	// verify that B is horizontal minimum (leftest-vertex)
	if(sina < 0) { B.x = -B.x; B.y = -B.y; }

	// if r2(ma) isn't in the horizontal range of
	// colliding with r1(r), collision is impossible
	if(B.x > TR.x || B.x > -BL.x) return false;

	// if r1(r) is axis-aligned, vertical min/max are easy to get
	if(t == 0) { ext1 = A.y; ext2 = -ext1; }
	// else, find vertical min/max in the range [BL.x, TR.x]
	else
	{
		x = BL.x - A.x; a = TR.x - A.x;
		ext1 = A.y;
		// if the first vertical min/max isn't in (BL.x, TR.x), then
		// find the vertical min/max on BL.x or on TR.x
		if(a*x > 0)
		{
			dx = A.x;
			if(x < 0) { dx -= B.x; ext1 -= B.y; x = a; }
			else { dx += B.x; ext1 += B.y; }
			ext1 *= x; ext1 /= dx; ext1 += A.y;
		}

		x = BL.x + A.x; a = TR.x + A.x;
		ext2 = -A.y;
		// if the second vertical min/max isn't in (BL.x, TR.x), then
		// find the local vertical min/max on BL.x or on TR.x
		if(a*x > 0)
		{
			dx = -A.x;
			if(x < 0) { dx -= B.x; ext2 -= B.y; x = a; }
			else { dx += B.x; ext2 += B.y; }
			ext2 *= x; ext2 /= dx; ext2 -= A.y;
		}
	}

	// check whether r2(ma) is in the vertical range of colliding with r1(r)
	// (for the horizontal range of r2)
	return !((ext1 < BL.y && ext2 < BL.y) ||
		(ext1 > TR.y && ext2 > TR.y));
}

// kolizja promienia (A->B) z cylindrem (P->Q, promie� R)
// z Real Time Collision Detection str 197
//----------------------------------------------
// Intersect segment S(t)=sa+t(sb-sa), 0<=t<=1 against cylinder specifiedby p, q and r
int RayToCylinder(const VEC3& sa, const VEC3& sb, const VEC3& p, const VEC3& q, float r, float& t)
{
	VEC3 d = q - p, m = sa - p, n = sb - sa;
	float md = m.Dot(d);
	float nd = n.Dot(d);
	float dd = d.Dot(d);
	// Test if segment fully outside either endcap of cylinder
	if(md < 0.0f && md + nd < 0.0f)
		return 0; // Segment outside 'p' side of cylinder
	if(md > dd && md + nd > dd)
		return 0; // Segment outside 'q' side of cylinder
	float nn = n.Dot(n);
	float mn = m.Dot(n);
	float a = dd * nn - nd * nd;
	float k = m.Dot(m) - r * r;
	float c = dd * k - md * md;
	if(is_zero(a))
	{
		// Segment runs parallel to cylinder axis
		if(c > 0.0f) return 0; // 'a' and thus the segment lie outside cylinder
							   // Now known that segment intersects cylinder; figure out how it intersects
		if(md < 0.0f) t = -mn / nn; // Intersect segment against 'p' endcap
		else if(md > dd) t = (nd - mn) / nn; // Intersect segment against 'q' endcap
		else t = 0.0f; // 'a' lies inside cylinder
		return 1;
	}
	float b = dd * mn - nd * md;
	float discr = b * b - a * c;
	if(discr < 0.0f) return 0; // No real roots; no intersection
	t = (-b - sqrt(discr)) / a;
	if(t < 0.0f || t > 1.0f) return 0; // Intersection lies outside segment
	if(md + t * nd < 0.0f)
	{
		// Intersection outside cylinder on 'p' side
		if(nd <= 0.0f) return 0; // Segment pointing away from endcap
		t = -md / nd;
		// Keep intersection if Dot(S(t) - p, S(t) - p) <= r /\ 2
		return k + 2 * t * (mn + t * nn) <= 0.0f;
	}
	else if(md + t * nd > dd)
	{
		// Intersection outside cylinder on 'q' side
		if(nd >= 0.0f) return 0; // Segment pointing away from endcap
		t = (dd - md) / nd;
		// Keep intersection if Dot(S(t) - q, S(t) - q) <= r /\ 2
		return k + dd - 2 * md + t * (2 * (mn - nd) + t * nn) <= 0.0f;
	}
	// Segment intersects cylinder between the endcaps; t is correct
	return 1;
}

struct MATRIX33
{
	VEC3 v[3];

	inline VEC3& operator [] (int n)
	{
		return v[n];
	}
};

// kolizja OOB z OOB
bool OOBToOOB(const OOB& a, const OOB& b)
{
	const float EPSILON = std::numeric_limits<float>::epsilon();

	float ra, rb;
	MATRIX33 R, AbsR;
	// Compute rotation matrix expressing b in a�s coordinate frame
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			R[i][j] = a.u[i].Dot(b.u[j]);
	// Compute translation vector t
	VEC3 t = b.c - a.c;
	// Bring translation into a�s coordinate frame
	t = VEC3(t.Dot(a.u[0]), t.Dot(a.u[2]), t.Dot(a.u[2]));
	// Compute common subexpressions. Add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null (see text for details)
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			AbsR[i][j] = abs(R[i][j]) + EPSILON;
	// Test axes L = A0, L = A1, L = A2
	for(int i = 0; i < 3; i++) {
		ra = a.e[i];
		rb = b.e[0] * AbsR[i][0] + b.e[1] * AbsR[i][1] + b.e[2] * AbsR[i][2];
		if(abs(t[i]) > ra + rb) return false;
	}
	// Test axes L = B0, L = B1, L = B2
	for(int i = 0; i < 3; i++) {
		ra = a.e[0] * AbsR[0][i] + a.e[1] * AbsR[1][i] + a.e[2] * AbsR[2][i];
		rb = b.e[i];
		if(abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return false;
	}
	// Test axis L = A0 x B0
	ra = a.e[1] * AbsR[2][0] + a.e[2] * AbsR[1][0];
	rb = b.e[1] * AbsR[0][2] + b.e[2] * AbsR[0][1];
	if(abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return false;
	// Test axis L = A0 x B1
	ra = a.e[1] * AbsR[2][1] + a.e[2] * AbsR[1][1];
	rb = b.e[0] * AbsR[0][2] + b.e[2] * AbsR[0][0];
	if(abs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb) return false;
	// Test axis L = A0 x B2
	ra = a.e[1] * AbsR[2][2] + a.e[2] * AbsR[1][2];
	rb = b.e[0] * AbsR[0][1] + b.e[1] * AbsR[0][0];
	if(abs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return false;
	// Test axis L = A1 x B0
	ra = a.e[0] * AbsR[2][0] + a.e[2] * AbsR[0][0];
	rb = b.e[1] * AbsR[1][2] + b.e[2] * AbsR[1][1];
	if(abs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return false;
	// Test axis L = A1 x B1
	ra = a.e[0] * AbsR[2][1] + a.e[2] * AbsR[0][1];
	rb = b.e[0] * AbsR[1][2] + b.e[2] * AbsR[1][0];
	if(abs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return false;
	// Test axis L = A1 x B2
	ra = a.e[0] * AbsR[2][2] + a.e[2] * AbsR[0][2];
	rb = b.e[0] * AbsR[1][1] + b.e[1] * AbsR[1][0];
	if(abs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return false;
	// Test axis L = A2 x B0
	ra = a.e[0] * AbsR[1][0] + a.e[1] * AbsR[0][0];
	rb = b.e[1] * AbsR[2][2] + b.e[2] * AbsR[2][1];
	if(abs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return false;
	// Test axis L = A2 x B1
	ra = a.e[0] * AbsR[1][1] + a.e[1] * AbsR[0][1];
	rb = b.e[0] * AbsR[2][2] + b.e[2] * AbsR[2][0];
	if(abs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return false;
	// Test axis L = A2 x B2
	ra = a.e[0] * AbsR[1][2] + a.e[1] * AbsR[0][2];
	rb = b.e[0] * AbsR[2][1] + b.e[1] * AbsR[2][0];
	if(abs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return false;
	// Since no separating axis is found, the OBBs must be intersecting
	return true;
}

float DistanceRectangleToPoint(const VEC2& pos, const VEC2& size, const VEC2& pt)
{
	float dx = max(abs(pt.x - pos.x) - size.x / 2, 0.f);
	float dy = max(abs(pt.y - pos.y) - size.y / 2, 0.f);
	return sqrt(dx * dx + dy * dy);
}

float PointLineDistance(float x0, float y0, float x1, float y1, float x2, float y2)
{
	float x = x2 - x1;
	float y = y2 - y1;
	return abs(y*x0 - x*y0 + x2*y1 - y2*x1) / sqrt(y*y + x*x);
}

float GetClosestPointOnLineSegment(const VEC2& A, const VEC2& B, const VEC2& P, VEC2& result)
{
	VEC2 AP = P - A;       //Vector from A to P   
	VEC2 AB = B - A;       //Vector from A to B  

	float magnitudeAB = AB.LengthSq(); //Magnitude of AB vector (it's length squared)     
	float ABAPproduct = AP.Dot(AB); //The DOT product of a_to_p and a_to_b     
	float distance = ABAPproduct / magnitudeAB; //The normalized "distance" from a to your closest point  

	if(distance < 0)     //Check if P projection is over vectorAB     
		result = A;
	else if(distance > 1)
		result = B;
	else
		result = A + AB * distance;

	return PointLineDistance(P.x, P.y, A.x, A.y, B.x, B.y);
}

const VEC2 POISSON_DISC_2D[] = {
	VEC2(-0.6271834f, -0.3647562f),
	VEC2(-0.6959124f, -0.1932297f),
	VEC2(-0.425675f, -0.4331925f),
	VEC2(-0.8259574f, -0.3775373f),
	VEC2(-0.4134415f, -0.2794108f),
	VEC2(-0.6711653f, -0.5842927f),
	VEC2(-0.505241f, -0.5710775f),
	VEC2(-0.5399489f, -0.1941965f),
	VEC2(-0.2056243f, -0.3328375f),
	VEC2(-0.2721521f, -0.4913186f),
	VEC2(0.009952361f, -0.4938473f),
	VEC2(-0.3341284f, -0.7402002f),
	VEC2(-0.009171869f, -0.1417411f),
	VEC2(-0.05370279f, -0.3561031f),
	VEC2(-0.2042215f, -0.1395438f),
	VEC2(0.1491909f, -0.7528881f),
	VEC2(-0.09437386f, -0.6736782f),
	VEC2(0.2218135f, -0.5837499f),
	VEC2(0.1357503f, -0.2823138f),
	VEC2(0.1759486f, -0.4372835f),
	VEC2(-0.8812768f, -0.1270963f),
	VEC2(-0.5861077f, -0.7143953f),
	VEC2(-0.4840448f, -0.8610057f),
	VEC2(-0.1953385f, -0.9313949f),
	VEC2(-0.3544169f, -0.1299241f),
	VEC2(0.4259588f, -0.3359875f),
	VEC2(0.1780135f, -0.006630601f),
	VEC2(0.3781602f, -0.174012f),
	VEC2(-0.6535406f, 0.07830032f),
	VEC2(-0.4176719f, 0.006290245f),
	VEC2(-0.2157413f, 0.1043319f),
	VEC2(-0.3825159f, 0.1611559f),
	VEC2(-0.04609891f, 0.1563928f),
	VEC2(-0.2525779f, 0.3147326f),
	VEC2(0.6283897f, -0.2800752f),
	VEC2(0.5242329f, -0.4569906f),
	VEC2(0.5337259f, -0.1482658f),
	VEC2(0.4243455f, -0.6266792f),
	VEC2(-0.8479414f, 0.08037262f),
	VEC2(-0.5815527f, 0.3148638f),
	VEC2(-0.790419f, 0.2343442f),
	VEC2(-0.4226354f, 0.3095743f),
	VEC2(-0.09465869f, 0.3677911f),
	VEC2(0.3935578f, 0.04151043f),
	VEC2(0.2390065f, 0.1743644f),
	VEC2(0.02775179f, 0.01711585f),
	VEC2(-0.3588479f, 0.4862351f),
	VEC2(-0.7332007f, 0.3809305f),
	VEC2(-0.5283061f, 0.5106883f),
	VEC2(0.7347565f, -0.04643056f),
	VEC2(0.5254471f, 0.1277963f),
	VEC2(-0.1984853f, 0.6903372f),
	VEC2(-0.1512452f, 0.5094652f),
	VEC2(-0.5878937f, 0.6584677f),
	VEC2(-0.4450369f, 0.7685395f),
	VEC2(0.691914f, -0.552465f),
	VEC2(0.293443f, -0.8303219f),
	VEC2(0.5147449f, -0.8018763f),
	VEC2(0.3373911f, -0.4752345f),
	VEC2(-0.7731022f, 0.6132235f),
	VEC2(-0.9054359f, 0.3877104f),
	VEC2(0.1200563f, -0.9095488f),
	VEC2(-0.05998399f, -0.8304204f),
	VEC2(0.1212275f, 0.4447584f),
	VEC2(-0.04844639f, 0.8149281f),
	VEC2(-0.1576151f, 0.9731216f),
	VEC2(-0.2921374f, 0.8280436f),
	VEC2(0.8305115f, -0.3373946f),
	VEC2(0.7025464f, -0.7087887f),
	VEC2(-0.9783711f, 0.1895637f),
	VEC2(-0.9950094f, 0.03602472f),
	VEC2(-0.02693105f, 0.6184058f),
	VEC2(-0.3686568f, 0.6363685f),
	VEC2(0.07644552f, 0.9160427f),
	VEC2(0.2174875f, 0.6892526f),
	VEC2(0.09518065f, 0.2284235f),
	VEC2(0.2566459f, 0.8855528f),
	VEC2(0.2196656f, -0.1571368f),
	VEC2(0.9549446f, -0.2014009f),
	VEC2(0.4562157f, 0.7741205f),
	VEC2(0.3333389f, 0.413012f),
	VEC2(0.5414181f, 0.2789065f),
	VEC2(0.7839744f, 0.2456573f),
	VEC2(0.6805856f, 0.1255756f),
	VEC2(0.3859844f, 0.2440029f),
	VEC2(0.4403853f, 0.600696f),
	VEC2(0.6249176f, 0.6072751f),
	VEC2(0.5145468f, 0.4502719f),
	VEC2(0.749785f, 0.4564187f),
	VEC2(0.9864355f, -0.0429658f),
	VEC2(0.8654963f, 0.04940263f),
	VEC2(0.9577024f, 0.1808657f)
};
const int poisson_disc_count = countof(POISSON_DISC_2D);

// D3DXMatrixMultiply (WINE)
void MATRIX::Multiply(MATRIX& out, const MATRIX& m1, const MATRIX& m2)
{	
	for(int i = 0; i<4; i++)
	{
		for(int j = 0; j<4; j++)
		{
			out(i,j) = m1(i,0) * m2(0,j) + m1(i,1) * m2(1,j) + m1(i,2) * m2(2,j) + m1(i,3) * m2(3,j);
		}
	}
}

// D3DXMatrixInverse (WINE)
bool MATRIX::Inverse(MATRIX& out, const MATRIX& m, float* determinant)
{
	FLOAT det, t[3], v[16];
	UINT i, j;
	
	t[0] = m(2,2) * m(3,3) - m(2,3) * m(3,2);
	t[1] = m(1,2) * m(3,3) - m(1,3) * m(3,2);
	t[2] = m(1,2) * m(2,3) - m(1,3) * m(2,2);
	v[0] = m(1,1) * t[0] - m(2,1) * t[1] + m(3,1) * t[2];
	v[4] = -m(1,0) * t[0] + m(2,0) * t[1] - m(3,0) * t[2];

	t[0] = m(1,0) * m(2,1) - m(2,0) * m(1,1);
	t[1] = m(1,0) * m(3,1) - m(3,0) * m(1,1);
	t[2] = m(2,0) * m(3,1) - m(3,0) * m(2,1);
	v[8] = m(3,3) * t[0] - m(2,3) * t[1] + m(1,3) * t[2];
	v[12] = -m(3,2) * t[0] + m(2,2) * t[1] - m(1,2) * t[2];

	det = m(0,0) * v[0] + m(0,1) * v[4] +
		m(0,2) * v[8] + m(0,3) * v[12];
	if(det == 0.0f)
		return false;
	if(determinant)
		*determinant = det;

	t[0] = m(2,2) * m(3,3) - m(2,3) * m(3,2);
	t[1] = m(0,2) * m(3,3) - m(0,3) * m(3,2);
	t[2] = m(0,2) * m(2,3) - m(0,3) * m(2,2);
	v[1] = -m(0,1) * t[0] + m(2,1) * t[1] - m(3,1) * t[2];
	v[5] = m(0,0) * t[0] - m(2,0) * t[1] + m(3,0) * t[2];

	t[0] = m(0,0) * m(2,1) - m(2,0) * m(0,1);
	t[1] = m(3,0) * m(0,1) - m(0,0) * m(3,1);
	t[2] = m(2,0) * m(3,1) - m(3,0) * m(2,1);
	v[9] = -m(3,3) * t[0] - m(2,3) * t[1] - m(0,3) * t[2];
	v[13] = m(3,2) * t[0] + m(2,2) * t[1] + m(0,2) * t[2];

	t[0] = m(1,2) * m(3,3) - m(1,3) * m(3,2);
	t[1] = m(0,2) * m(3,3) - m(0,3) * m(3,2);
	t[2] = m(0,2) * m(1,3) - m(0,3) * m(1,2);
	v[2] = m(0,1) * t[0] - m(1,1) * t[1] + m(3,1) * t[2];
	v[6] = -m(0,0) * t[0] + m(1,0) * t[1] - m(3,0) * t[2];

	t[0] = m(0,0) * m(1,1) - m(1,0) * m(0,1);
	t[1] = m(3,0) * m(0,1) - m(0,0) * m(3,1);
	t[2] = m(1,0) * m(3,1) - m(3,0) * m(1,1);
	v[10] = m(3,3) * t[0] + m(1,3) * t[1] + m(0,3) * t[2];
	v[14] = -m(3,2) * t[0] - m(1,2) * t[1] - m(0,2) * t[2];

	t[0] = m(1,2) * m(2,3) - m(1,3) * m(2,2);
	t[1] = m(0,2) * m(2,3) - m(0,3) * m(2,2);
	t[2] = m(0,2) * m(1,3) - m(0,3) * m(1,2);
	v[3] = -m(0,1) * t[0] + m(1,1) * t[1] - m(2,1) * t[2];
	v[7] = m(0,0) * t[0] - m(1,0) * t[1] + m(2,0) * t[2];

	v[11] = -m(0,0) * (m(1,1) * m(2,3) - m(1,3) * m(2,1)) +
		m(1,0) * (m(0,1) * m(2,3) - m(0,3) * m(2,1)) -
		m(2,0) * (m(0,1) * m(1,3) - m(0,3) * m(1,1));

	v[15] = m(0,0) * (m(1,1) * m(2,2) - m(1,2) * m(2,1)) -
		m(1,0) * (m(0,1) * m(2,2) - m(0,2) * m(2,1)) +
		m(2,0) * (m(0,1) * m(1,2) - m(0,2) * m(1,1));

	det = 1.0f / det;

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			out(i,j) = v[4 * i + j] * det;

	return true;
}

// D3DXMatrixTransformation2D (WINE)
MATRIX MATRIX::Transformation2D(const VEC2* scaling_center, float scaling_rotation, const VEC2* scaling, const VEC2* rotation_center, float rotation,
	const VEC2* translation)
{
	QUAT rot, sca_rot;
	VEC3 rot_center, sca, sca_center, trans;
	
	if(scaling_center)
	{
		sca_center.x = scaling_center->x;
		sca_center.y = scaling_center->y;
		sca_center.z = 0.0f;
	}
	else
	{
		sca_center.x = 0.0f;
		sca_center.y = 0.0f;
		sca_center.z = 0.0f;
	}

	if(scaling)
	{
		sca.x = scaling->x;
		sca.y = scaling->y;
		sca.z = 1.0f;
	}
	else
	{
		sca.x = 1.0f;
		sca.y = 1.0f;
		sca.z = 1.0f;
	}

	if(rotation_center)
	{
		rot_center.x = rotation_center->x;
		rot_center.y = rotation_center->y;
		rot_center.z = 0.0f;
	}
	else
	{
		rot_center.x = 0.0f;
		rot_center.y = 0.0f;
		rot_center.z = 0.0f;
	}

	if(translation)
	{
		trans.x = translation->x;
		trans.y = translation->y;
		trans.z = 0.0f;
	}
	else
	{
		trans.x = 0.0f;
		trans.y = 0.0f;
		trans.z = 0.0f;
	}

	rot.w = cosf(rotation / 2.0f);
	rot.x = 0.0f;
	rot.y = 0.0f;
	rot.z = sinf(rotation / 2.0f);

	sca_rot.w = cosf(scaling_rotation / 2.0f);
	sca_rot.x = 0.0f;
	sca_rot.y = 0.0f;
	sca_rot.z = sinf(scaling_rotation / 2.0f);

	return Transformation(&sca_center, &sca_rot, &sca, &rot_center, &rot, &trans);
}

// D3DXMatrixTransformation (WINE)
MATRIX MATRIX::Transformation(const VEC3* scaling_center, const QUAT* scaling_rotation, const VEC3* scaling, const VEC3* rotation_center,
	const QUAT* rotation, const VEC3* translation)
{
	QUAT prc;
	VEC3 psc, pt;
	
	if(!scaling_center)
	{
		psc.x = 0.0f;
		psc.y = 0.0f;
		psc.z = 0.0f;
	}
	else
	{
		psc.x = scaling_center->x;
		psc.y = scaling_center->y;
		psc.z = scaling_center->z;
	}

	if(!rotation_center)
	{
		prc.x = 0.0f;
		prc.y = 0.0f;
		prc.z = 0.0f;
	}
	else
	{
		prc.x = rotation_center->x;
		prc.y = rotation_center->y;
		prc.z = rotation_center->z;
	}

	if(!translation)
	{
		pt.x = 0.0f;
		pt.y = 0.0f;
		pt.z = 0.0f;
	}
	else
	{
		pt.x = translation->x;
		pt.y = translation->y;
		pt.z = translation->z;
	}

	MATRIX m = Translation(-psc.x, -psc.y, -psc.z),
		m_rot,
		m_rot_inv;

	if(!scaling_rotation)
	{
		m_rot.Identity();
		m_rot_inv.Identity();
	}
	else
	{
		m_rot = Rotation(*scaling_rotation);
		m_rot_inv = m_rot.Inverse();
	}
	
	m *= m_rot_inv;
	if(scaling)
		m *= Scaling(*scaling);
	m *= m_rot;
	m *= Translation(psc.x - prc.x, psc.y - prc.y, psc.z - prc.z);
	if(rotation)
		m *= Rotation(*rotation);
	m *= Translation(prc.x + pt.x, prc.y + pt.y, prc.z + pt.z);
	return m;
}

// D3DXMatrixRotationYawPitchRoll (WINE)
MATRIX MATRIX::Rotation(float yaw, float pitch, float roll)
{
	float sroll, croll, spitch, cpitch, syaw, cyaw;
	sroll = sinf(roll);
	croll = cosf(roll);
	spitch = sinf(pitch);
	cpitch = cosf(pitch);
	syaw = sinf(yaw);
	cyaw = cosf(yaw);

	MATRIX m;
	m(0,0) = sroll * spitch * syaw + croll * cyaw;
	m(0,1) = sroll * cpitch;
	m(0,2) = sroll * spitch * cyaw - croll * syaw;
	m(0,3) = 0.0f;
	m(1,0) = croll * spitch * syaw - sroll * cyaw;
	m(1,1) = croll * cpitch;
	m(1,2) = croll * spitch * cyaw + sroll * syaw;
	m(1,3) = 0.0f;
	m(2,0) = cpitch * syaw;
	m(2,1) = -spitch;
	m(2,2) = cpitch * cyaw;
	m(2,3) = 0.0f;
	m(3,0) = 0.0f;
	m(3,1) = 0.0f;
	m(3,2) = 0.0f;
	m(3,3) = 1.0f;
	return m;
}

// D3DXMatrixLookAtLH (WINE)
MATRIX MATRIX::LookAt(const VEC3& eye, const VEC3& at, const VEC3& up)
{
	VEC3 right, upn, vec;
	
	vec = at - eye;
	vec.Normalized();
	right = up.Cross(vec);
	upn = vec.Cross(right);
	right.Normalized();
	upn.Normalized();

	MATRIX m;
	m(0,0) = right.x;
	m(1,0) = right.y;
	m(2,0) = right.z;
	m(3,0) = -right.Dot(eye);
	m(0,1) = upn.x;
	m(1,1) = upn.y;
	m(2,1) = upn.z;
	m(3,1) = -upn.Dot(eye);
	m(0,2) = vec.x;
	m(1,2) = vec.y;
	m(2,2) = vec.z;
	m(3,2) = -vec.Dot(eye);
	m(0,3) = 0.0f;
	m(1,3) = 0.0f;
	m(2,3) = 0.0f;
	m(3,3) = 1.0f;
	return m;
}

// D3DXMatrixPerspectiveFovLH (WINE)
MATRIX MATRIX::PerspectiveFov(float fovy, float aspect, float z_near, float z_far)
{
	MATRIX m;
	m.Identity();
	m(0,0) = 1.0f / (aspect * tanf(fovy / 2.0f));
	m(1,1) = 1.0f / tanf(fovy / 2.0f);
	m(2,2) = z_far / (z_far - z_near);
	m(2,3) = 1.0f;
	m(3,2) = (z_far * z_near) / (z_near - z_far);
	m(3,3) = 0.0f;
	return m;
}

// D3DXQuaternionRotationMatrix (WINE)
QUAT QUAT::FromMatrix(const MATRIX& m)
{
	float s, trace;
	QUAT q;
	
	trace = m(0,0) + m(1,1) + m(2,2) + 1.0f;
	if(trace > 1.0f)
	{
		s = 2.0f * sqrtf(trace);
		q.x = (m(1,2) - m(2,1)) / s;
		q.y = (m(2,0) - m(0,2)) / s;
		q.z = (m(0,1) - m(1,0)) / s;
		q.w = 0.25f * s;
	}
	else
	{
		int i, maxi = 0;

		for(i = 1; i < 3; i++)
		{
			if(m(i,i) > m(maxi,maxi))
				maxi = i;
		}

		switch(maxi)
		{
		case 0:
			s = 2.0f * sqrtf(1.0f + m(0,0) - m(1,1) - m(2,2));
			q.x = 0.25f * s;
			q.y = (m(0,1) + m(1,0)) / s;
			q.z = (m(0,2) + m(2,0)) / s;
			q.w = (m(1,2) - m(2,1)) / s;
			break;

		case 1:
			s = 2.0f * sqrtf(1.0f + m(1,1) - m(0,0) - m(2,2));
			q.x = (m(0,1) + m(1,0)) / s;
			q.y = 0.25f * s;
			q.z = (m(1,2) + m(2,1)) / s;
			q.w = (m(2,0) - m(0,2)) / s;
			break;

		case 2:
			s = 2.0f * sqrtf(1.0f + m(2,2) - m(0,0) - m(1,1));
			q.x = (m(0,2) + m(2,0)) / s;
			q.y = (m(1,2) + m(2,1)) / s;
			q.z = 0.25f * s;
			q.w = (m(0,1) - m(1,0)) / s;
			break;
		}
	}

	return q;
}

// D3DXVec2TransformCoord (WINE)
VEC2 MATRIX::TransformCoord(const VEC2& v) const
{
	VEC2 out = v;
	const MATRIX& m = *this;
	float norm = m(0,3) * v.x + m(1,3) * v.y + m(3,3);

	out.x = (m(0,0) * v.x + m(1,0) * v.y + m(3,0)) / norm;
	out.y = (m(0,1) * v.x + m(1,1) * v.y + m(3,1)) / norm;

	return out;
}

// D3DXVec3Transform (WINE)
VEC4 MATRIX::Transform(const VEC3& v) const
{
	VEC4 out;
	const MATRIX& m = *this;
	
	out.x = m(0,0) * v.x + m(1,0) * v.y + m(2,0) * v.z + m(3,0);
	out.y = m(0,1) * v.x + m(1,1) * v.y + m(2,1) * v.z + m(3,1);
	out.z = m(0,2) * v.x + m(1,2) * v.y + m(2,2) * v.z + m(3,2);
	out.w = m(0,3) * v.x + m(1,3) * v.y + m(2,3) * v.z + m(3,3);

	return out;
}

// D3DXVec3TransformCoord (WINE)
VEC3 MATRIX::TransformCoord(const VEC3& v) const
{
	VEC3 out = v;
	const MATRIX& m = *this;
	float norm = m(0,3) * v.x + m(1,3) * v.y + m(2,3) * v.z + m(3,3);

	out.x = (m(0,0) * v.x + m(1,0) * v.y + m(2,0) * v.z + m(3,0)) / norm;
	out.y = (m(0,1) * v.x + m(1,1) * v.y + m(2,1) * v.z + m(3,1)) / norm;
	out.z = (m(0,2) * v.x + m(1,2) * v.y + m(2,2) * v.z + m(3,2)) / norm;

	return out;
}

// D3DXVec3TransformNormal (WINE)
VEC3 MATRIX::TransformNormal(const VEC3& v) const
{
	VEC3 out = v;
	const MATRIX& m = *this;

	out.x = m(0,0) * v.x + m(1,0) * v.y + m(2,0) * v.z;
	out.y = m(0,1) * v.x + m(1,1) * v.y + m(2,1) * v.z;
	out.z = m(0,2) * v.x + m(1,2) * v.y + m(2,2) * v.z;

	return out;
}
