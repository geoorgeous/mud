#include "transformations.hpp"

#include <cmath>

#include "../quaternion.hpp"
#include "../vector/vector_3.hpp"
#include "../vector/vector_4.hpp"
#include "matrix_4.hpp"

namespace mud
{
	Matrix4 transform_t(float tX, float tY, float tZ)
	{
		return {
			Vector4(1.0f, 0.0f, 0.0f, 0.0f),
			Vector4(0.0f, 1.0f, 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, 1.0f, 0.0f),
			Vector4(tX, tY, tZ, 1.0f)
		};
	}

	Matrix4 transform_t(const Vector3& v)
	{
		return transform_t(v.x, v.y, v.z);
	}

	Matrix4 transform_s(float sX, float sY, float sZ)
	{
		return {
			Vector4(sX, 0.0f, 0.0f, 0.0f),
			Vector4(0.0f, sY, 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, sZ, 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		};
	}

	Matrix4 transform_s(const Vector3& v)
	{
		return transform_s(v.x, v.y, v.z);
	}

	Matrix4 transform_r(const Quaternion& q)
	{
		float xx = q.x * q.x;
		float xy = q.x * q.y;
		float xz = q.x * q.z;
		float xw = q.x * q.w;

		float yy = q.y * q.y;
		float yz = q.y * q.z;
		float yw = q.y * q.w;

		float zz = q.z * q.z;
		float zw = q.z * q.w;

		return {
			Vector4(1.0f - 2.0f * (yy + zz), 2.0f * (xy - zw), 2.0f * (xz + yw), 0.0f),
			Vector4(2.0f * (xy + zw), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - xw), 0.0f),
			Vector4(2.0f * (xz - yw), 2.0f * (yz + xw), 1.0f - 2.0f * (xx + yy), 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		};
	}

	Matrix4 transform_rX(float radians)
	{
		return {
			Vector4(1.0f, 0.0f, 0.0f, 0.0f),
			Vector4(0.0f, std::cos(radians), std::sin(radians), 0.0f),
			Vector4(0.0f, -std::sin(radians), std::cos(radians), 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		};
	}

	Matrix4 transform_rY(float radians)
	{
		return {
			Vector4(std::cos(radians), 0.0f, -std::sin(radians), 0.0f),
			Vector4(0.0f, 1.0f, 0.0f, 0.0f),
			Vector4(std::sin(radians), 0.0f, std::cos(radians), 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		};
	}

	Matrix4 transform_rZ(float radians)
	{
		return {
			Vector4(std::cos(radians), std::sin(radians), 0.0f, 0.0f),
			Vector4(-std::sin(radians), std::cos(radians), 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, 1.0f, 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		};
	}

	void transformDecompose(Matrix4 t, Vector3 & translation, Vector3 & scale, Vector3 & rotation)
	{
		translation = Vector3(t.columns[3]);
		scale.x = Vector3(t.columns[0]).magnitude();
		scale.y = Vector3(t.columns[1]).magnitude();
		scale.z = Vector3(t.columns[2]).magnitude();

		// Remove scaling to prepare for extraction of rotation

		if (scale.x != 0)
		{
			t[0][0] /= scale.x;
			t[0][1] /= scale.x;
			t[0][2] /= scale.x;
		}
		if (scale.y != 0)
		{
			t[1][0] /= scale.y;
			t[1][1] /= scale.y;
			t[1][2] /= scale.y;
		}
		if (scale.z != 0)
		{
			t[2][0] /= scale.z;
			t[2][1] /= scale.z;
			t[2][2] /= scale.z;
		}
		t[3][3] = 1.0f;

		// Verify orientation, if necessary invert it.
		Vector3 zAxis = Vector3(t.columns[0]).cross(Vector3(t.columns[1]));
		if (zAxis.dot(Vector3(t.columns[2])) < 0)
		{
			scale.x *= -1.0f;
			t[0][0] *= -1.0f;
			t[0][1] *= -1.0f;
			t[0][2] *= -1.0f;
		}

		// http ://www.insomniacgames.com/mike-day-extracting-euler-angles-from-a-rotation-matrix/
		float theta1 = std::atan2(t[1][2], t[2][2]);
		float c2 = std::sqrt(t[0][0] * t[0][0] + t[0][1] * t[0][1]);
		float theta2 = std::atan2(-t[0][2], c2);
		float s1 = std::sin(theta1);
		float c1 = std::cos(theta1);
		float theta3 = std::atan2(s1 * t[2][0] - c1 * t[1][0], c1 * t[1][1] - s1 * t[2][1]);
		rotation = Vector3(-theta1, -theta2, -theta3);
	}

	Matrix4 perspective(float verticalFOV, float aspect, float zNear, float zFar)
	{
		const float zRange = zNear - zFar;
		const float f = 1.0f / tanf(verticalFOV * 0.5f);
		return Matrix4{
			Vector4(f / aspect, 0.0f, 0.0f, 0.0f),
			Vector4(0.0f, f, 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, (zFar + zNear) / zRange, -1.0f),
			Vector4(0.0f, 0.0f, (2 * zFar * zNear) / zRange, 0.0f)
		};
	}

	Matrix4 orthographic(float left, float right, float top, float bottom, float zNear, float zFar)
	{
		return Matrix4{
			Vector4(2.0f / (right - left), 0.0f, 0.0f, 0.0f),
			Vector4(0.0f, 2.0f / (top - bottom), 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, -2.0f / (zFar - zNear), 0.0f),
			Vector4(-((right + left) / (right - left)), -((top + bottom) / (top - bottom)), -((zFar + zNear) / (zFar - zNear)), 1.0f)
		};
	}

	Matrix4 view(const Vector3& right, const Vector3& up, const Vector3& forward, const Vector3& eye)
	{
		return Matrix4{
			Vector4(right.x, right.y, right.z, 0.0f),
			Vector4(up.x, up.y, up.z, 0.0f),
			Vector4(-forward.x, -forward.y, -forward.z, 0.0f), // Vulkan is right-handed
			Vector4(-eye.x, -eye.y, -eye.z, 1.0f)
		};
	}

	Matrix4 lookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
	{
		Vector3 f = (eye - target).normal();
		Vector3 s = up.cross(f).normal();
		Vector3 u = f.cross(s);

		return Matrix4{
			Vector4(s.x, u.x, f.x, 0.0f),
			Vector4(s.y, u.y, f.y, 0.0f),
			Vector4(s.z, u.z, f.z, 0.0f),
			Vector4(-s.dot(eye), -u.dot(eye), -f.dot(eye), 1.0f)
		};
	}
}