#include "quaternion.hpp"

#include <math.h>

#include "vector/vector_3.hpp"

namespace mud
{
	Quaternion Quaternion::identity = { 1, 0, 0, 0 };

	Vector3 Quaternion::xyz() const
	{
		return Vector3{ x, y, z };
	}

	float Quaternion::sqrMagnitude() const
	{
		return w * w + x * x + y * y + z * z;
	}

	float Quaternion::magnitude() const
	{
		return sqrtf(sqrMagnitude());
	}

	Quaternion Quaternion::conjugate() const
	{
		return {
			w,
			-x,
			-y,
			-z
		};
	}

	Quaternion Quaternion::normal() const
	{
		float m = magnitude();
		return Quaternion{ w / m, x / m, y / m, z / m };
	}

	Quaternion& Quaternion::normalise()
	{
		return *this = this->normal();
	}

	Quaternion Quaternion::inverse() const
	{
		float sqrM = sqrMagnitude();
		return Quaternion{
			w / sqrM,
			-x / sqrM,
			-y / sqrM,
			-z / sqrM
		};
	}

	Vector3 Quaternion::getEulerAngles() const
	{
		return Vector3(
			atan2f(2.0f * (w * x + y* z), 1.0f - 2.0f * (x * x + y * y)),
			asinf(-2.0f * (w * y - z * x)),
			atan2f(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z)));
	}

	Quaternion& Quaternion::operator=(const Quaternion& rhs)
	{
		w = rhs.w;
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}

	Quaternion& Quaternion::operator+=(const Quaternion& rhs)
	{
		w += rhs.w;
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Quaternion& Quaternion::operator-=(const Quaternion& rhs)
	{
		w -= rhs.w;
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	Quaternion& Quaternion::operator*=(const Quaternion& rhs)
	{
		return *this = *this * rhs;
	}

	Quaternion Quaternion::fromEulerAngles(const Vector3& eulerAngles)
	{
		float cy = cosf(eulerAngles.z * 0.5f);
		float sy = sinf(eulerAngles.z * 0.5f);
		float cp = cosf(eulerAngles.y * 0.5f);
		float sp = sinf(eulerAngles.y * 0.5f);
		float cr = cosf(eulerAngles.x * 0.5f);
		float sr = sinf(eulerAngles.x * 0.5f);

		return {
			cy * cp * cr + sy * sp * sr,
			cy * cp * sr - sy * sp * cr,
			sy * cp * sr + cy * sp * cr,
			sy * cp * cr - cy * sp * sr
		};
	}

	Quaternion axisAngle(float angle, const Vector3& axis, bool normaliseAxis)
	{
		float halfAngle = angle * 0.5f;
		float sinHalfAngle = sinf(halfAngle);
		Vector3 v = (normaliseAxis) ? axis.normal() : axis;

		return {
			cosf(halfAngle),
			v.x * sinHalfAngle,
			v.y * sinHalfAngle,
			v.z * sinHalfAngle
		};
	}
}