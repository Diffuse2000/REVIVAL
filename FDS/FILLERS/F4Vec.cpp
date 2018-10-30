#include "F4Vec.h"

/*F4Vec::F4Vec(void)
{
	f1 = f2 = f3 = f4 = 0.0f;
}

F4Vec::F4Vec(float _f1, float _f2, float _f3, float _f4)
{
	f1 = _f1;
	f2 = _f2;
	f3 = _f3;
	f4 = _f4;
}

F4Vec::F4Vec(F4Vec &Vec)
{
	*this = Vec;
}

F4Vec::~F4Vec(void)
{
}*/

void F4Vec::assign(float _f1, float _f2, float _f3, float _f4)
{
	f1 = _f1;
	f2 = _f2;
	f3 = _f3;
	f4 = _f4;
}

/*F4Vec & F4Vec::operator =(F4Vec &Vec)
{
	f1 = Vec.f1;
	f2 = Vec.f2;
	f3 = Vec.f3;
	f4 = Vec.f4;

	return *this;
}*/

F4Vec operator +(F4Vec Vec1, F4Vec &Vec2)
{
	Vec1.f1 += Vec2.f1;
	Vec1.f2 += Vec2.f2;
	Vec1.f3 += Vec2.f3;
	Vec1.f4 += Vec2.f4;
	return Vec1;
}

F4Vec operator -(F4Vec Vec1, F4Vec &Vec2)
{
	Vec1.f1 -= Vec2.f1;
	Vec1.f2 -= Vec2.f2;
	Vec1.f3 -= Vec2.f3;
	Vec1.f4 -= Vec2.f4;
	return Vec1;
}

F4Vec operator *(F4Vec Vec1, F4Vec &Vec2)
{
	Vec1.f1 *= Vec2.f1;
	Vec1.f2 *= Vec2.f2;
	Vec1.f3 *= Vec2.f3;
	Vec1.f4 *= Vec2.f4;
	return Vec1;
}

F4Vec operator /(F4Vec Vec1, F4Vec &Vec2)
{
	Vec1.f1 /= Vec2.f1;
	Vec1.f2 /= Vec2.f2;
	Vec1.f3 /= Vec2.f3;
	Vec1.f4 /= Vec2.f4;
	return Vec1;
}

/*F4Vec & F4Vec::operator =(float _f1)
{
	f1 = f2 = f3 = f4 = _f1;
	return *this;
}*/

F4Vec operator +(F4Vec Vec1, float _f1)
{
	Vec1.f1 += _f1;
	Vec1.f2 += _f1;
	Vec1.f3 += _f1;
	Vec1.f4 += _f1;
	return Vec1;
}

F4Vec operator -(F4Vec Vec1, float _f1)
{
	Vec1.f1 -= _f1;
	Vec1.f2 -= _f1;
	Vec1.f3 -= _f1;
	Vec1.f4 -= _f1;
	return Vec1;
}

F4Vec operator *(F4Vec Vec1, float _f1)
{
	Vec1.f1 *= _f1;
	Vec1.f2 *= _f1;
	Vec1.f3 *= _f1;
	Vec1.f4 *= _f1;
	return Vec1;
}

F4Vec operator /(F4Vec Vec1, float _f1)
{
	Vec1.f1 /= _f1;
	Vec1.f2 /= _f1;
	Vec1.f3 /= _f1;
	Vec1.f4 /= _f1;
	return Vec1;
}

F4Vec & F4Vec::operator +=(F4Vec &Vec)
{
	f1 += Vec.f1;
	f2 += Vec.f2;
	f3 += Vec.f3;
	f4 += Vec.f4;
	return *this;
}

F4Vec & F4Vec::operator -=(F4Vec &Vec)
{
	f1 -= Vec.f1;
	f2 -= Vec.f2;
	f3 -= Vec.f3;
	f4 -= Vec.f4;
	return *this;
}

F4Vec & F4Vec::operator *=(F4Vec &Vec)
{
	f1 *= Vec.f1;
	f2 *= Vec.f2;
	f3 *= Vec.f3;
	f4 *= Vec.f4;
	return *this;
}

F4Vec & F4Vec::operator /=(F4Vec &Vec)
{
	f1 /= Vec.f1;
	f2 /= Vec.f2;
	f3 /= Vec.f3;
	f4 /= Vec.f4;
	return *this;
}

F4Vec & F4Vec::operator +=(float _f1)
{
	f1 += _f1;
	f2 += _f1;
	f3 += _f1;
	f4 += _f1;
	return *this;
}

F4Vec & F4Vec::operator -=(float _f1)
{
	f1 -= _f1;
	f2 -= _f1;
	f3 -= _f1;
	f4 -= _f1;
	return *this;
}

F4Vec & F4Vec::operator *=(float _f1)
{
	f1 *= _f1;
	f2 *= _f1;
	f3 *= _f1;
	f4 *= _f1;
	return *this;
}

F4Vec & F4Vec::operator /=(float _f1)
{
	f1 /= _f1;
	f2 /= _f1;
	f3 /= _f1;
	f4 /= _f1;
	return *this;
}
