#include "matrix_4.hpp"

#include <cmath>

#include "../quaternion.hpp"
#include "../vector/vector_3.hpp"
#include "matrix_2.hpp"
#include "matrix_3.hpp"

namespace mud
{
	const Matrix4 Matrix4::zero{
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	};

	const Matrix4 Matrix4::identity{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	Matrix4::Matrix4(float diagonal)
		: columns{
			Vector4(diagonal, 0.0f, 0.0f, 0.0f),
			Vector4(0.0f, diagonal, 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, diagonal, 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, diagonal)
	} { }

	Matrix4::Matrix4(float a, float b, float c, float d,
		float e, float f, float g, float h,
		float i, float j, float k, float l,
		float m, float n, float o, float p)
		: columns{
			Vector4(a, b, c, d),
			Vector4(e, f, g, h),
			Vector4(i, j, k, l),
			Vector4(m, n, o, p)
	} { }

	Matrix4::Matrix4(const Vector4& col0, const Vector4& col1, const Vector4& col2, const Vector4& col3)
		: columns{ col0, col1, col2, col3 }
	{ }

	Matrix4::Matrix4(const Matrix2& mat2)
		: columns{
			Vector4(mat2[0][0], mat2[0][1], 0.0f, 0.0f),
			Vector4(mat2[1][0], mat2[1][1], 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, 0.0f)
	} { }

	Matrix4::Matrix4(const Matrix3& mat3)
		: columns{
			Vector4(mat3[0][0], mat3[0][1], mat3[0][2], 0.0f),
			Vector4(mat3[1][0], mat3[1][1], mat3[1][2], 0.0f),
			Vector4(mat3[2][0], mat3[2][1], mat3[2][2], 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, 0.0f)
	} { }

	Vector4 Matrix4::getRow(unsigned int idx) const
	{
		return Vector4{
			columns[0][idx],
			columns[1][idx],
			columns[2][idx],
			columns[3][idx]
		};
	}

	Matrix4 Matrix4::transpose() const
	{
		return { getRow(0), getRow(1), getRow(2), getRow(3) };
	}

	Matrix4 Matrix4::inverse() const
	{
		return (1.0f / determinant()) * Matrix4 {
			Matrix3 {
				columns[1][1], columns[1][2], columns[1][3],
				columns[2][1], columns[2][2], columns[2][3],
				columns[3][1], columns[3][2], columns[3][3] }.determinant(),
			-Matrix3{
				columns[1][0], columns[1][2], columns[1][3],
				columns[2][0], columns[2][2], columns[2][3],
				columns[3][0], columns[3][2], columns[3][3] }.determinant(),
			Matrix3{
				columns[1][0], columns[1][1], columns[1][3],
				columns[2][0], columns[2][1], columns[2][3],
				columns[3][0], columns[3][1], columns[3][3] }.determinant(),
			-Matrix3{
				columns[1][0], columns[1][1], columns[1][2],
				columns[2][0], columns[2][1], columns[2][2],
				columns[3][0], columns[3][1], columns[3][2] }.determinant(),

			-Matrix3 {
				columns[0][1], columns[0][2], columns[0][3],
				columns[2][1], columns[2][2], columns[2][3],
				columns[3][1], columns[3][2], columns[3][3] }.determinant(),
			Matrix3{
				columns[0][0], columns[0][2], columns[0][3],
				columns[2][0], columns[2][2], columns[2][3],
				columns[3][0], columns[3][2], columns[3][3] }.determinant(),
			-Matrix3{
				columns[0][0], columns[0][1], columns[0][3],
				columns[2][0], columns[2][1], columns[2][3],
				columns[3][0], columns[3][1], columns[3][3] }.determinant(),
			Matrix3{
				columns[0][0], columns[0][1], columns[0][2],
				columns[2][0], columns[2][1], columns[2][2],
				columns[3][0], columns[3][1], columns[3][2] }.determinant(),

			Matrix3 {
				columns[0][1], columns[0][2], columns[0][3],
				columns[1][1], columns[1][2], columns[1][3],
				columns[3][1], columns[3][2], columns[3][3] }.determinant(),
			-Matrix3{
				columns[0][0], columns[0][2], columns[0][3],
				columns[1][0], columns[1][2], columns[1][3],
				columns[3][0], columns[3][2], columns[3][3] }.determinant(),
			Matrix3{
				columns[0][0], columns[0][1], columns[0][3],
				columns[1][0], columns[1][1], columns[1][3],
				columns[3][0], columns[3][1], columns[3][3] }.determinant(),
			-Matrix3{
				columns[0][0], columns[0][1], columns[0][2],
				columns[1][0], columns[1][1], columns[1][2],
				columns[3][0], columns[3][1], columns[3][2] }.determinant(),

			-Matrix3 {
				columns[0][1], columns[0][2], columns[0][3],
				columns[1][1], columns[1][2], columns[1][3],
				columns[2][1], columns[2][2], columns[2][3] }.determinant(),
			Matrix3{
				columns[0][0], columns[0][2], columns[0][3],
				columns[1][0], columns[1][2], columns[1][3],
				columns[2][0], columns[2][2], columns[2][3] }.determinant(),
			-Matrix3{
				columns[0][0], columns[0][1], columns[0][3],
				columns[1][0], columns[1][1], columns[1][3],
				columns[2][0], columns[2][1], columns[2][3] }.determinant(),
			Matrix3{
				columns[0][0], columns[0][1], columns[0][2],
				columns[1][0], columns[1][1], columns[1][2],
				columns[2][0], columns[2][1], columns[2][2] }.determinant()
		}.transpose();
	}

	float Matrix4::determinant() const
	{
		float d = 0.0f;
		int c, subi, subj, i, j;
		Matrix3 submat;
		for (c = 0; c < 4; c++)
		{
			subi = 0;
			for (i = 1; i < 4; i++)
			{
				subj = 0;
				for (j = 0; j < 4; j++)
				{
					if (j == c)
						continue;
					submat[subi][subj] = columns[i][j];
					subj++;
				}
				subi++;
			}
			d = d + (std::pow(-1, c) * columns[0][c] * submat.determinant());
		}
		return d;
	}

	Vector4& Matrix4::operator[](unsigned int idx)
	{
		return columns[idx];
	}

	const Vector4& Matrix4::operator[](unsigned int idx) const
	{
		return columns[idx];
	}

	Matrix4& Matrix4::operator+=(const Matrix4& rhs)
	{
		for (int idx = 0; idx < 4; idx++)
			columns[idx] += rhs.columns[idx];
		return *this;
	}

	Matrix4& Matrix4::operator-=(const Matrix4& rhs)
	{
		for (int idx = 0; idx < 4; idx++)
			columns[idx] -= rhs.columns[idx];
		return *this;
	}

	Matrix4& Matrix4::operator*=(float s)
	{
		for (int idx = 0; idx < 4; idx++)
			columns[idx] *= s;
		return *this;
	}

	Matrix4& Matrix4::operator/=(float s)
	{
		for (int idx = 0; idx < 4; idx++)
			columns[idx] /= s;
		return *this;
	}

	Matrix4 Matrix4::operator-() const
	{
		return *this * -1.0f;
	}
}