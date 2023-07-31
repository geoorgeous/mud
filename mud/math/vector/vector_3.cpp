#include "vector_3.hpp"

#include <cmath>

#include "../quaternion.hpp"
#include "vector_2.hpp"
#include "vector_4.hpp"

namespace mud
{
	const Vector3 Vector3::zero{ 0.0f, 0.0f, 0.0f };
	const Vector3 Vector3::one{ 1.0f, 1.0f, 1.0f };
	const Vector3 Vector3::posX{ 1.0f, 0.0f, 0.0f };
	const Vector3 Vector3::negX{ -1.f, 0.0f, 0.0f };
	const Vector3 Vector3::posY{ 0.0f, 1.0f, 0.0f };
	const Vector3 Vector3::negY{ 0.0f, -1.f, 0.0f };
	const Vector3 Vector3::posZ{ 0.0f, 0.0f, 1.0f };
	const Vector3 Vector3::negZ{ 0.0f, 0.0f, -1.f };

	Vector3::Vector3(float f)
		: x(f), y(f), z(f)
	{ }

	Vector3::Vector3(float x, float y, float z)
		: x(x), y(y), z(z)
	{ }

	Vector3::Vector3(const Vector2& vec2)
		: x(vec2.x), y(vec2.y)
	{ }

	Vector3::Vector3(const Vector4& vec4)
		: x(vec4.x), y(vec4.y), z(vec4.z)
	{ }

	float Vector3::magnitudeSq() const
	{
		return x * x + y * y + z * z;
	}

	float Vector3::magnitude() const
	{
		return std::sqrt(magnitudeSq());
	}

	float Vector3::dot(const Vector3& v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	float Vector3::distance(const Vector3& v) const
	{
		return std::sqrt(dot(*this - v));
	}

	float Vector3::angle(const Vector3& v) const
	{
		return std::acos(dot(v) / std::sqrt(magnitudeSq() * v.magnitudeSq()));
	}

	Vector3 Vector3::cross(const Vector3& v) const
	{
		return {
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
		};
	}

	Vector3 Vector3::normal() const
	{
		float m = magnitude();
		return{ x / m, y / m, z / m };
	}

	Vector3& Vector3::normalise()
	{
		return *this = normal();
	}

	Vector3& Vector3::limit(float newMagnitude)
	{
		float m = magnitude();
		x = x * newMagnitude / m;
		y = y * newMagnitude / m;
		z = z * newMagnitude / m;
		return *this;
	}

	float& Vector3::operator[](unsigned int idx)
	{
		return ((float*)this)[idx];
	}

	const float& Vector3::operator[](unsigned int idx) const
	{
		return ((float*)this)[idx];
	}

	Vector3& Vector3::operator=(const Vector3& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}

	Vector3& Vector3::operator+=(const Vector3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Vector3& Vector3::operator-=(const Vector3& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	Vector3& Vector3::operator*=(const Vector3& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}

	Vector3& Vector3::operator/=(const Vector3& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		return *this;
	}

	Vector3& Vector3::operator*=(float s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	Vector3& Vector3::operator/=(float s)
	{
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}

	Vector3& Vector3::operator*=(const Quaternion& rhs)
	{
		return *this = *this * rhs;
	}

	Vector3 Vector3::operator-() const
	{
		return *this * -1.0f;
	}
}