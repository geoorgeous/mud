#include "vector_4.hpp"

#include <cmath>

#include "vector_2.hpp"
#include "vector_3.hpp"

namespace mud
{
	const Vector4 Vector4::zero{ 0.0f, 0.0f, 0.0f, 0.0f };

	const Vector4 Vector4::one{ 1.0f, 1.0f, 1.0f, 1.0f };

	Vector4::Vector4(float f)
		: x(f), y(f), z(f), w(f)
	{ }

	Vector4::Vector4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w)
	{ }

	Vector4::Vector4(const Vector2& vec2, float z, float w)
		: x(vec2.x), y(vec2.y), z(z), w(w)
	{ }

	Vector4::Vector4(const Vector3& vec3, float w)
		: x(vec3.x), y(vec3.y), z(vec3.z), w(w)
	{ }

	float Vector4::magnitudeSq() const
	{
		return x * x + y * y + z * z + w * w;
	}

	float Vector4::magnitude() const
	{
		return std::sqrt(magnitudeSq());
	}

	float Vector4::dot(const Vector4& v) const
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	Vector4 Vector4::normal() const
	{
		float m = magnitude();
		return{ x / m, y / m, z / m, w / m };
	}

	Vector4& Vector4::normalise()
	{
		return *this = normal();
	}

	Vector4& Vector4::limit(float newMagnitude)
	{
		float m = magnitude();
		x = x * newMagnitude / m;
		y = y * newMagnitude / m;
		z = z * newMagnitude / m;
		w = w * newMagnitude / m;
		return *this;
	}

	float& Vector4::operator[](unsigned int idx)
	{
		return ((float*)this)[idx];
	}

	const float& Vector4::operator[](unsigned int idx) const
	{
		return ((float*)this)[idx];
	}

	Vector4& Vector4::operator=(const Vector4& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;
		return *this;
	}

	Vector4& Vector4::operator+=(const Vector4& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}

	Vector4& Vector4::operator-=(const Vector4& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}

	Vector4& Vector4::operator*=(const Vector4& rhs)
	{

		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		w *= rhs.w;
		return *this;
	}

	Vector4& Vector4::operator/=(const Vector4& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		w /= rhs.w;
		return *this;
	}

	Vector4& Vector4::operator*=(float s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}

	Vector4& Vector4::operator/=(float s)
	{
		x /= s;
		y /= s;
		z /= s;
		w /= s;
		return *this;
	}

	Vector4 Vector4::operator-() const
	{
		return *this * -1.0f;
	}
}