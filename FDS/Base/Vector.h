
#ifndef REVIVAL_VECTOR_H
#define REVIVAL_VECTOR_H

#include <stdio.h>

#pragma pack(push, 1)

// [12 Bytes]
struct Vector
{
	float x, y, z;

	Vector() {}
	Vector(float _x, float _y, float _z): x(_x), y(_y), z(_z) {}
	~Vector() {}

	void print() {
		printf("(%f, %f, %f)", x, y, z);
	}

	bool is_zero() {
		return 0.0f == x && x == y && y == z;
	}

/////////////////////////////////////////////////////////
	inline static void add(Vector &u, Vector &v, Vector &w)
	{
		w.x = u.x + v.x;
		w.y = u.y + v.y;
		w.z = u.z + v.z;
	}

	// this = u + v;
	inline void add(Vector &u, Vector &v)
	{
		x = u.x + v.x;
		y = u.y + v.y;
		z = u.z + v.z;
	}

	inline Vector operator + (const Vector &u) const
	{
		return Vector(x + u.x, y + u.y, z + u.z);
	}

	inline Vector &operator += (const Vector &u)
	{
		x += u.x;
		y += u.y;
		z += u.z;
		return *this;
	}

/////////////////////////////////////////////////////////
	inline static void sub(Vector &u, Vector &v, Vector &w)
	{
		w.x = u.x - v.x;
		w.y = u.y - v.y;
		w.z = u.z - v.z;
	}

	// this = u - v;
	inline void sub(Vector &u, Vector &v)
	{
		x = u.x - v.x;
		y = u.y - v.y;
		z = u.z - v.z;
	}

	inline Vector operator - (const Vector &u) const
	{
		return Vector(x - u.x, y - u.y, z - u.z);
	}

	inline Vector &operator -= (const Vector &u)
	{
		x -= u.x;
		y -= u.y;
		z -= u.z;
		return *this;
	}

/////////////////////////////////////////////////////////
	inline static void cross(Vector &u, Vector &v, Vector &w)
	{
		w.x = u.y * v.z - u.z * v.y;
		w.y = u.z * v.x - u.x * v.z;
		w.z = u.x * v.y - u.y * v.x;
	}

	inline void cross(Vector &u, Vector &v)
	{
		x = u.y * v.z - u.z * v.y;
		y = u.z * v.x - u.x * v.z;
		z = u.x * v.y - u.y * v.x;
	}

	inline Vector operator ^ (const Vector &v) const
	{

		return Vector(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x);
	}

	inline Vector &operator ^= (const Vector &v)
	{
		Vector	t(x, y, z);
		x = t.y * v.z - t.z * v.y;
		y = t.z * v.x - t.x * v.z;
		z = t.x * v.y - t.y * v.x;
		return *this;
	}
/////////////////////////////////////////////////////////
	inline static float dot(Vector &u, Vector &v)
	{
		return (u.x * v.x + u.y * v.y + u.z * v.z);
	}

	inline float operator * (const Vector &u) const
	{
		return (u.x * x + u.y * y + u.z * z);
	}


/////////////////////////////////////////////////////////
	Vector &operator *= (float s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	Vector &operator /= (float s)
	{
		float r = 1/s;
		x *= r;
		y *= r;
		z *= r;
		return *this;
	}
	Vector operator * (float s) const
	{
		return Vector(x*s, y*s, z*s);
	}

	Vector operator / (float s) const
	{
		float r = 1/s;
		return Vector(x*r, y*r, z*r);
	}

	friend Vector operator * (float s, Vector &v);
};

inline Vector operator * (float s, Vector &v)
{
	return Vector(v.x * s, v.y * s, v.z * s);
}

#pragma pack(pop)

#endif //REVIVAL_VECTOR_H
