#pragma once
#include "Protocol.pb.h"

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3() : x(0), y(0), z(0) {}

	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3(const Protocol::PBVector& v)
	{
		this->x = v.x();
		this->y = v.y();
		this->z = v.z();
	}

	bool operator==(const Vector3& v)
	{
		return x == v.x && y == v.y && z == v.z;
	}

	Vector3 operator-(const Vector3& o)
	{
		Vector3 r;
		r.x = x - o.x;
		r.y = y - o.y;
		r.z = z - o.z;
		return r;
	}

	bool operator!=(const Vector3& o)
	{
		// 같지 않아야함
		return this->x != o.x || this->y != o.y;
	}

	Vector3 operator+(const Vector3& o)
	{
		Vector3 r;
		r.x = x + o.x;
		r.y = y + o.y;
		r.z = z + o.z;
		return r;
	}

	Vector3& Normalize()
	{
		auto c = 1.0f / std::sqrt(x * x + y * y + z * z);
		x *= c;
		y *= c; 
		z *= c;
		return *this;
	}

	float Dot(const Vector3& o)
	{
		return x * o.x + y * o.y +z * o.z;
	}

	float Cross(const Vector3& o)
	{
		return x * o.y - y * o.x ;
	}

	Protocol::PBVector ConvertPBVector()
	{
		Protocol::PBVector r;
		r.set_x(this->x);
		r.set_y(this->y);
		r.set_z(this->z);
		return r;
	}
};


//
//
//struct Vector3
//{
//public:
//
//public:
//	float x;
//	float y;
//	float z;
//};