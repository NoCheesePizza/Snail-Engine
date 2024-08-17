#include "Vec2.h"
#include "Utility.h"

#include <cmath>
#include <algorithm>

namespace Snail
{

	Vec2::Vec2(float _x, float _y)
		: x(_x), y(_y)
	{

	}

	std::string Vec2::stringify() const
	{
		return "{ x: " + toStr(x) + "; y: " + toStr(y) + " }";
	}

	Vec2 Vec2::operator+(Vec2 vec) const
	{
		return { x + vec.x, y + vec.y };
	}

	Vec2 &Vec2::operator+=(Vec2 vec)
	{
		x += vec.x;
		y += vec.y;
		return *this;
	}

	Vec2 Vec2::operator-(Vec2 vec) const
	{
		return { x - vec.x, y - vec.y };
	}

	Vec2 &Vec2::operator-=(Vec2 vec)
	{
		x -= vec.x;
		y -= vec.y;
		return *this;
	}

	Vec2 Vec2::operator*(float val) const
	{
		return { x * val, y * val };
	}

	Vec2 &Vec2::operator*=(float val)
	{
		x *= val;
		y *= val;
		return *this;
	}

	Vec2 Vec2::operator/(float val) const
	{
		return { x / val, y / val };
	}

	Vec2 &Vec2::operator/=(float val)
	{
		x /= val;
		y /= val;
		return *this;
	}

	Vec2 Vec2::operator+(float val) const
	{
		return { x + val, y + val };
	}

	Vec2 &Vec2::operator+=(float val)
	{
		x += val;
		y += val;
		return *this;
	}

	Vec2 Vec2::operator-(float val) const
	{
		return { x - val, y - val };
	}

	Vec2 &Vec2::operator-=(float val)
	{
		x -= val;
		y -= val;
		return *this;
	}

	Vec2 Vec2::operator*(Vec2 vec) const
	{
		return { x * vec.x, y * vec.y };
	}

	Vec2 &Vec2::operator*=(Vec2 vec)
	{
		x *= vec.x;
		y *= vec.y;
		return *this;
	}

	Vec2 Vec2::operator/(Vec2 vec) const
	{
		return { x / vec.x, y / vec.y };
	}

	Vec2 &Vec2::operator/=(Vec2 vec)
	{
		x /= vec.x;
		y /= vec.y;
		return *this;
	}

	Vec2 Vec2::operator^(int val) const
	{
		return { std::powf(x, static_cast<float>(val)), std::powf(y, static_cast<float>(val)) };
	}

	Vec2 &Vec2::operator^=(int val)
	{
		x = std::powf(x, static_cast<float>(val));
		y = std::powf(y, static_cast<float>(val));
		return *this;
	}

	Vec2 Vec2::operator-() const
	{
		return { -x, -y };
	}

	Vec2 Vec2::operator++() const
	{
		float larger = std::max(x, y);
		return { larger, larger };
	}

	Vec2 Vec2::operator--() const
	{
		float smaller = std::min(x, y);
		return { smaller, smaller };
	}

	bool Vec2::operator>(Vec2 vec) const
	{
		return x > vec.x && y > vec.y;
	}

	bool Vec2::operator>=(Vec2 vec) const
	{
		return *this > vec || *this == vec;
	}

	bool Vec2::operator<(Vec2 vec) const
	{
		return x < vec.x && y < vec.y;
	}

	bool Vec2::operator<=(Vec2 vec) const
	{
		return *this < vec || *this == vec;
	}

	bool Vec2::operator==(Vec2 vec) const
	{
		return Util::isEqual(x, vec.x) && Util::isEqual(y, vec.y);
	}

	Vec2 Vec2::normalize() const
	{
		float len = length();
		if (len)
			return { x / len, y / len };
		return Vec2();
	}

	Vec2 Vec2::outwardNormal() const
	{
		return { y, -x };
	}

	Vec2 Vec2::inwardNormal() const
	{
		return { -y, x };
	}

	float Vec2::length() const
	{
		return std::sqrtf(squareLength());
	}

	float Vec2::squareLength() const
	{
		return x * x + y * y;
	}

	float Vec2::distance(Vec2 vec) const
	{
		return std::sqrtf(squareDistance(vec));
	}

	float Vec2::squareDistance(Vec2 vec) const
	{
		return std::powf(x - vec.x, 2.f) + std::powf(y - vec.y, 2.f);
	}

	float Vec2::dot(Vec2 vec) const
	{
		return x * vec.x + y * vec.y;
	}

	float Vec2::cross(Vec2 vec) const
	{
		return x * vec.y - y * vec.x;
	}

	Vec2 Vec2::midpoint(Vec2 vec) const
	{
		return { (x + vec.x) / 2.f, (y + vec.y) / 2.f };
	}

}