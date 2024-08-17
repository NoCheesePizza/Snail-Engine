#pragma once

#include "Debug.h"

namespace Snail
{

	class Vec2 : public Debugger::Printable
	{
	public:

		float x, y;

		Vec2(float _x = 0.f, float _y = 0.f);

		std::string stringify() const override;

		Vec2 operator+(Vec2 vec) const;
		Vec2 &operator+=(Vec2 vec);
		Vec2 operator-(Vec2 vec) const;
		Vec2 &operator-=(Vec2 vec);

		Vec2 operator*(float val) const;
		Vec2 &operator*=(float val);
		Vec2 operator/(float val) const;
		Vec2 &operator/=(float val);

		Vec2 operator+(float val) const;
		Vec2 &operator+=(float val);
		Vec2 operator-(float val) const;
		Vec2 &operator-=(float val);

		Vec2 operator*(Vec2 vec) const;
		Vec2 &operator*=(Vec2 vec);
		Vec2 operator/(Vec2 vec) const;
		Vec2 &operator/=(Vec2 vec);

		Vec2 operator^(int val) const;
		Vec2 &operator^=(int val);
		
		Vec2 operator-() const;
		Vec2 operator++() const;
		Vec2 operator--() const;

		bool operator>(Vec2 vec) const;
		bool operator>=(Vec2 vec) const;
		bool operator<(Vec2 vec) const;
		bool operator<=(Vec2 vec) const;
		bool operator==(Vec2 vec) const;

		Vec2 normalize() const;
		Vec2 outwardNormal() const;
		Vec2 inwardNormal() const;

		float length() const;
		float squareLength() const;
		float distance(Vec2 vec) const;
		float squareDistance(Vec2 vec) const;

		float dot(Vec2 vec) const;
		float cross(Vec2 vec) const;
		Vec2 midpoint(Vec2 vec) const;
	};

}