#include "matrix_3.hpp"

#include "matrix_2.hpp"
#include "matrix_4.hpp"

namespace mud
{
	const Matrix3 Matrix3::zero {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};

	const Matrix3 Matrix3::identity{
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	Matrix3::Matrix3(float diagonal)
		: columns{
			Vector3(diagonal, 0.0f, 0.0f),
			Vector3(0.0f, diagonal, 0.0f),
			Vector3(0.0f, 0.0f, diagonal)
	} { }

	Matrix3::Matrix3(float a, float b, float c,
		float d, float e, float f,
		float g, float h, float i)
		: columns{
			Vector3(a, b, c),
			Vector3(d, e, f),
			Vector3(g, h, i)
	} { }

	Matrix3::Matrix3(const Vector3& col0, const Vector3& col1, const Vector3& col2)
		: columns{ col0, col1, col2 }
	{ }

	Matrix3::Matrix3(const Matrix2& mat2)
		: columns{
			Vector3(mat2[0][0], mat2[0][1], 0.0f),
			Vector3(mat2[1][0], mat2[1][1], 0.0f),
			Vector3(0.0f, 0.0f, 0.0f)
	} { }

	Matrix3::Matrix3(const Matrix4& mat4)
		: columns{
			Vector3(mat4[0][0], mat4[0][1], mat4[0][2]),
			Vector3(mat4[1][0], mat4[1][1], mat4[1][2]),
			Vector3(mat4[2][0], mat4[2][1], mat4[2][2]),
	} { }

	Vector3 Matrix3::getRow(unsigned int idx) const
	{
		return Vector3{
			columns[0][idx],
			columns[1][idx],
			columns[2][idx]
		};
	}

	Matrix3 Matrix3::transpose() const
	{
		return {
			Vector3(columns[0][0], columns[1][0], columns[2][0]),
			Vector3(columns[0][1], columns[1][1], columns[2][1]),
			Vector3(columns[0][2], columns[1][2], columns[2][2])
		};
	}

	Matrix3 Matrix3::inverse() const
	{
		return (1.0f / determinant()) * Matrix3(
			Vector3(
				columns[1][1] * columns[2][2] - columns[2][1] * columns[1][2],
				-(columns[1][0] * columns[2][2] - columns[2][0] * columns[1][2]),
				columns[1][0] * columns[2][1] - columns[2][0] * columns[1][1]),
			Vector3(
				-(columns[0][1] * columns[2][2] - columns[2][1] * columns[0][2]),
				columns[0][0] * columns[2][2] - columns[2][0] * columns[0][2],
				-(columns[0][0] * columns[2][1] - columns[2][0] * columns[0][1])),
			Vector3(
				columns[0][1] * columns[1][2] - columns[1][1] * columns[0][2],
				-(columns[0][0] * columns[1][2] - columns[1][0] * columns[0][2]),
				columns[0][0] * columns[1][1] - columns[1][0] * columns[0][1])).transpose();
	}

	float Matrix3::determinant() const
	{
		return (columns[0][0] * (columns[1][1] * columns[2][2] - columns[2][1] * columns[1][2])) -
			(columns[1][0] * (columns[0][1] * columns[2][2] - columns[2][1] * columns[0][2])) +
			(columns[2][0] * (columns[0][1] * columns[1][2] - columns[1][1] * columns[0][2]));
	}

	Vector3& Matrix3::operator[](unsigned int idx)
	{
		return columns[idx];
	}

	const Vector3& Matrix3::operator[](unsigned int idx) const
	{
		return columns[idx];
	}

	Matrix3& Matrix3::operator+=(const Matrix3& rhs)
	{
		for (int idx = 0; idx < 3; idx++)
			columns[idx] += rhs.columns[idx];
		return *this;
	}

	Matrix3& Matrix3::operator-=(const Matrix3& rhs)
	{
		for (int idx = 0; idx < 3; idx++)
			columns[idx] -= rhs.columns[idx];
		return *this;
	}

	Matrix3& Matrix3::operator*=(float s)
	{
		for (int idx = 0; idx < 3; idx++)
			columns[idx] *= s;
		return *this;
	}

	Matrix3& Matrix3::operator/=(float s)
	{
		for (int idx = 0; idx < 3; idx++)
			columns[idx] /= s;
		return *this;
	}

	Matrix3 Matrix3::operator-() const
	{
		return *this * -1.0f;
	}
}