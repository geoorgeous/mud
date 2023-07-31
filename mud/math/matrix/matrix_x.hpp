#ifndef MUD_MATRIX
#define MUD_MATRIX

#include "../vector/vector_x.hpp"

namespace mud
{
	template<typename T, unsigned int Major, unsigned int Minor = Major>
	struct Matrix
	{
		using MatT = Matrix<T, Major, Minor>;
		using ColT = Vector<T, Minor>;
		using RowT = Vector<T, Major>;

		ColT columns[Major];

		Matrix() = default;

		RowT getRow(unsigned int idx) const
		{
			RowT v;
			for (unsigned int col = 0; col < Major; col++)
				v[col] = columns[col][idx];
			return v;
		}

		MatT transpose() const
		{
			MatT m;
			for (unsigned int origCol = 0; origCol < Major; origCol++)
				for (unsigned int origRow = 0; origRow < Minor; origRow++)
					m[origRow][origCol] = columns[origCol][origRow];
			return m;
		}

		template<typename = typename std::enable_if<Major == Minor>::type>
		MatT minor()
		{
			if (Major == 1)
				return *this;

			MatT m;
			if (Major == 2)
			{
				m[0][0] = this->columns[1][1];
				m[0][1] = this->columns[1][0];
				m[1][0] = this->columns[0][1];
				m[1][1] = this->columns[0][0];
				return m;
			}

			for (unsigned int col = 0; col < Major; col++)
				for (unsigned int row = 0; row < Major; row++)
				{
					Matrix<T, Major - 1> subM;
					unsigned int subMCol = 0, subMRow = 0;
					for (unsigned int col2 = 0; col2 < Major; col2++)
					{
						if (col == col2)
							continue;
						subMRow = 0;
						for (unsigned int row2 = 0; row2 < Major; row2++)
						{
							if (row == row2)
								continue;
							subM[subMCol][subMRow] = this->columns[col2][row2];
							subMRow++;
						}
						subMCol++;
					}
					m[col][row] = subM.determinant();
				}
			return m;
		}

		template<typename = typename std::enable_if<Major == Minor>::type>
		MatT cofactor()
		{
			MatT m = minor();
			for (unsigned int col = 0; col < Major; col++)
				for (unsigned int row = 0; row < Major; row++)
					if ((col + row) % 2 != 0)
						m[col][row] *= -1.0f;
			return m;
		}

		template<typename = typename std::enable_if<Major == Minor>::type>
		MatT adjoint()
		{
			return cofactor().transpose();
		}

		template<typename = typename std::enable_if<Major == Minor>::type>
		MatT inverse()
		{
			return (1.0f / determinant()) * adjoint();
		}

		template<typename = typename std::enable_if<Major == Minor>::type>
		float determinant() const
		{
			if (Major == 2)
				return((this->columns[0][0] * this->columns[1][1]) - (this->columns[1][0] * this->columns[0][1]));
			if (Major == 1)
				return this->columns[0][0];

			float d = 0.0f;
			int c, subi, subj, i, j;
			Matrix<T, (Major < 2) ? 1 : Major - 1> submat;
			for (c = 0; c < Major; c++)
			{
				subi = 0;
				for (i = 1; i < Major; i++)
				{
					subj = 0;
					for (j = 0; j < Major; j++)
					{
						if (j == c)
							continue;
						submat[subi][subj] = this->columns[i][j];
						subj++;
					}
					subi++;
				}
				d = d + (std::pow(-1, c) * this->columns[0][c] * submat.determinant());
			}
			return d;
		}

		ColT& operator[](unsigned int idx)
		{
			return columns[idx];
		}

		const ColT& operator[](unsigned int idx) const
		{
			return columns[idx];
		}

		MatT& operator+=(const Matrix<T, Major, Minor>& rhs)
		{
			for (unsigned int idx = 0; idx < Major; idx++)
				columns[idx] += rhs.columns[idx];
			return *this;
		}

		MatT& operator-=(const Matrix<T, Major, Minor>& rhs)
		{
			for (unsigned int idx = 0; idx < Major; idx++)
				columns[idx] -= rhs.columns[idx];
			return *this;
		}

		MatT& operator*=(float s)
		{
			for (unsigned int idx = 0; idx < Major; idx++)
				columns[idx] *= s;
			return *this;
		}

		MatT& operator/=(float s)
		{
			for (unsigned int idx = 0; idx < Major; idx++)
				columns[idx] /= s;
			return *this;
		}

		MatT operator-() const
		{
			return *this * -1.0f;
		}
	};

	template<typename T, unsigned int Major, unsigned int Minor = Major>
	inline Matrix<T, Major, Minor> operator+(Matrix<T, Major, Minor> lhs, const Matrix<T, Major, Minor>& rhs)
	{
		return lhs += rhs;
	}

	template<typename T, unsigned int Major, unsigned int Minor = Major>
	inline Matrix<T, Major, Minor> operator-(Matrix<T, Major, Minor> lhs, const Matrix<T, Major, Minor>& rhs)
	{
		return lhs -= rhs;
	}

	template<typename T, unsigned int Major, unsigned int Minor = Major>
	inline Matrix<T, Major, Minor> operator*(Matrix<T, Major, Minor> lhs, float s)
	{
		return lhs *= s;
	}

	template<typename T, unsigned int Major, unsigned int Minor = Major>
	inline Matrix<T, Major, Minor> operator*(float s, Matrix<T, Major, Minor> rhs)
	{
		return rhs *= s;
	}

	template<typename T, unsigned int Major, unsigned int Minor = Major>
	inline Matrix<T, Major, Minor> operator/(Matrix<T, Major, Minor> lhs, float s)
	{
		return lhs /= s;
	}

	template<typename T, unsigned int RowsLHS, unsigned int N, unsigned int ColsRHS>
	inline Matrix<T, ColsRHS, RowsLHS> operator*(const Matrix<T, N, RowsLHS>& lhs, const Matrix<T, ColsRHS, N>& rhs)
	{
		Matrix<T, ColsRHS, RowsLHS> result;
		for (unsigned int col = 0; col < ColsRHS; col++)
			for (unsigned int row = 0; row < RowsLHS; row++)
				result[col][row] = lhs.getRow(row).dot(rhs.columns[col]);
		return result;
	}

	template<typename T, unsigned int Major, unsigned int Minor = Major>
	inline Vector<T, Minor> operator*(const Matrix<T, Major, Minor>& lhs, const Vector<T, Minor>& rhs)
	{
		Vector<T, Minor> v;
		for (unsigned int idx = 0; idx < Minor; idx++)
			v[idx] = lhs.getRow(idx).dot(rhs);
	}

	template<typename T, unsigned int Major, unsigned int Minor = Major>
	inline bool operator==(const Matrix<T, Major, Minor>& lhs, const Matrix<T, Major, Minor>& rhs)
	{
		for (unsigned int idx = 0; idx < Major; idx++)
			if (lhs[idx] != rhs[idx])
				return false;
		return true;
	}

	template<typename T, unsigned int Major, unsigned int Minor = Major>
	inline bool operator!=(const Matrix<T, Major, Minor>& lhs, const Matrix<T, Major, Minor>& rhs)
	{
		return !(lhs == rhs);
	}
}

#endif