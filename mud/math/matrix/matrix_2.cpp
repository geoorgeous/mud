#include "matrix_2.hpp"

#include "matrix_3.hpp"
#include "matrix_4.hpp"

namespace mud
{
	const Matrix2 Matrix2::zero{
		0.0f, 0.0f,
		0.0f, 0.0f
	};

	const Matrix2 Matrix2::identity{
		1.0f, 0.0f,
		0.0f, 1.0f
	};

	Matrix2::Matrix2(float diagonal)
		: columns{
			Vector2(diagonal, 0.0f),
			Vector2(0.0f, diagonal)
	} { }

	Matrix2::Matrix2(float a, float b,
		float c, float d)
		: columns{
			Vector2(a, b),
			Vector2(c, d)
	} { }

	Matrix2::Matrix2(const Vector2& col0, const Vector2& col1)
		: columns{ col0, col1 }
	{ }

	Matrix2::Matrix2(const Matrix3& mat3)
		: columns{
			Vector2(mat3[0][0], mat3[0][1]),
			Vector2(mat3[1][0], mat3[1][1])
	} { }

	Matrix2::Matrix2(const Matrix4& mat4)
		: columns{
			Vector2(mat4[0][0], mat4[0][1]),
			Vector2(mat4[1][0], mat4[1][1])
	} { }

	Vector2 Matrix2::getRow(unsigned int idx) const
	{
		return Vector2{
			columns[0][idx],
			columns[1][idx]
		};
	}

	Matrix2 Matrix2::transpose() const
	{
		return {
			Vector2(columns[0][0], columns[1][0]),
			Vector2(columns[0][1], columns[1][1])
		};
	}

	Matrix2 Matrix2::inverse() const
	{
		return (1.0f / determinant()) * Matrix2 { 
			Vector2(columns[1][1], -columns[0][1]), 
			Vector2(-columns[1][0], columns[0][0])
		};
	}

	float Matrix2::determinant() const
	{
		return columns[0][0] * columns[1][1] - columns[1][0] * columns[0][1];
	}

	Vector2& Matrix2::operator[](unsigned int idx)
	{
		return columns[idx];
	}

	const Vector2& Matrix2::operator[](unsigned int idx) const
	{
		return columns[idx];
	}

	Matrix2& Matrix2::operator+=(const Matrix2& rhs)
	{
		for (int idx = 0; idx < 2; idx++)
			columns[idx] += rhs.columns[idx];
		return *this;
	}

	Matrix2& Matrix2::operator-=(const Matrix2& rhs)
	{
		for (int idx = 0; idx < 2; idx++)
			columns[idx] -= rhs.columns[idx];
		return *this;
	}

	Matrix2& Matrix2::operator*=(float s)
	{
		for (int idx = 0; idx < 2; idx++)
			columns[idx] *= s;
		return *this;
	}

	Matrix2& Matrix2::operator/=(float s)
	{
		for (int idx = 0; idx < 2; idx++)
			columns[idx] /= s;
		return *this;
	}

	Matrix2 Matrix2::operator-() const
	{
		return *this * -1.0f;
	}
}