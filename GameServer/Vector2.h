#pragma once

struct Vector2
{
	float x;
	float y;

	Vector2() : x(0), y(0) {}

	Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	bool operator==(const Vector2& v)
	{
		return x == v.x && y == v.y;
	}

	Vector2 operator-(const Vector2& o)
	{
		Vector2 r;
		r.x = x - o.x;
		r.y = y - o.y;
		return r;
	}

	Vector2 operator+(const Vector2& o)
	{
		Vector2 r;
		r.x = x + o.x;
		r.y = y + o.y;
		return r;
	}

	Vector2& Normalize()
	{
		auto c = 1.0f / std::sqrt(x * x + y * y);
		x *= c;
		y *= c;
		return *this;
	}

	float Dot(const Vector2& o)
	{
		return x * o.x + y * o.y;
	}

	float Cross(const Vector2& o)
	{
		return x * o.y - y * o.x;
	}
};

