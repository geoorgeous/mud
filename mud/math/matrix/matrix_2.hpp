/**
 * \file
 * \author \link https://georgemcdonagh.co.uk George McDonagh
 */

#pragma once

#include "../vector/vector_2.hpp"

namespace mud
{
	struct Matrix3;
	struct Matrix4;

	/**
	 * \brief A mathematical 2 x 2 matrix of floating-point values.
	 * \details This struct is used to represent square matrices with 2 columns and 2 rows.
	 * \note All GML matrix data is stored in column-major order, i.e.: the contiguous memory of a matrix is stored column after column. You may often need a pointer to this data, the best way to get this is to simply use <code>&someMatrix[0][0];</code>. This provides a pointer to the matrix's first element.
	 */
	struct Matrix2
	{
		static const Matrix2 zero; //!< A static Matrix2 with all elements set to 0.0f.
		static const Matrix2 identity; //!< A static Matrix2 with all diagonal elements set to 1.0f, AKA the Identity matrix.

		Vector2 columns[2]; //!< The columns of the Matrix2.

		/**
		 * \brief Default constructor. Elements of the Matrix2 are initialised to 0.0f by default.
		 */
		Matrix2() = default;

		/**
		 * \brief Constructs a Matrix2 with all diagonal elements set to a specific value.
		 * \param diagonal The value given to the diagonal elements of the Matrix2.
		 */
		Matrix2(float diagonal);

		/**
		 * \brief Constructs a Matrix2 with specified values.
		 * \param a The first column's first element. (The first row's first element)
		 * \param b The first column's second element. (The second row's first element)
		 * \param c The second column's first element. (The first row's second element)
		 * \param d The second column's second element. (The second row's second element)
		 */
		Matrix2(float a, float b,
			float c, float d);

		/**
		 * \brief Constructs a Matrix2 with specified columns.
		 * \param col0 The first column of the Matrix2.
		 * \param col1 The second column of the Matrix2.
		 */
		Matrix2(const Vector2& col0, const Vector2& col1);

		/**
		 * \brief Constructs a Matrix2 from a Matrix3.
		 * \details The top-left 2 x 2 sub-matrix of the Matrix3 is used to set the elements of the Matrix2.
		 * \param mat3 The Matrix3 to copy elements from.
		 */
		Matrix2(const Matrix3& mat3);

		/**
		 * \brief Constructs a Matrix2 from a Matrix2.
		 * \details The top-left 2 x 2 sub-matrix of the Matrix4 is used to set the elements of the Matrix2.
		 * \param mat4 The Matrix4 to copy elements from.
		 */
		Matrix2(const Matrix4& mat4);

		/**
		 * \brief Constructs and returns a specific row of the Matrix2.
		 * \param idx The index of the row to return.
		 * \returns A Vector2 comprised of the elements of the specified row.
		 * \warning This function performs no array-bounds checking. the passed index must be [0 alvere/math 1].
		 */
		Vector2 getRow(unsigned int idx) const;

		/**
		 * \brief Calculates and returns the transpose matrix of the Matrix2.
		 * \details The transpose of a matrix is the same matrix but with elements of rows and columns switch around. In other words the matrix rows become the columns and the columns become the rows.
		 * \returns A Matrix2 that is the transpose of the original Matrix2.
		 */
		Matrix2 transpose() const;

		/**
		 * \brief Calculates and returns the inverse matrix of the Matrix2.
		 * \details In 3D graphics, the inverse of a matrix is useful to reverse the original matrix's transformation.
		 * \returns A Matrix2 that is the inverse of the original Matrix2.
		 */
		Matrix2 inverse() const;

		/**
		 * \brief Calculates and returns the determinant of the Matrix2.
		 * \returns The determinant of the Matrix2.
		 */
		float determinant() const;

		/**
		 * \brief Accesses the columns of the Matrix2 using a given index.
		 * \param idx The specified index of the column to return.
		 * \returns A modifiable reference to the column found at the specified index.
		 * \warning This function performs no array-bounds checking. the passed index must be [0 alvere/math 1].
		 */
		Vector2& operator[](unsigned int idx);

		/**
		 * \brief Accesses the columns of the Matrix2 using a given index.
		 * \param idx The specified index of the column to return.
		 * \returns An immutable reference to the column found at the specified index.
		 * \warning This function performs no array-bounds checking. the passed index must be [0 alvere/math 1].
		 */
		const Vector2& operator[](unsigned int idx) const;

		/**
		 * \brief Adds another Matrix2 to the Matrix2.
		 * \details Addition is performed element-wise.
		 * \param rhs A Matrix2 that will be added to the Matrix2 on the left.
		 * \returns A modifiable reference to the Matrix2 after the addition.
		 */
		Matrix2& operator+=(const Matrix2& rhs);

		/**
		 * \brief Subtracts another Matrix2 from the Matrix2.
		 * \details Subtraction is performed element-wise.
		 * \param rhs A Matrix2 that will be subtracted away from the Matrix3 on the left.
		 * \returns A modifiable reference to the Matrix2 after the subtraction.
		 */
		Matrix2& operator-=(const Matrix2& rhs);

		/**
		 * \brief Multiplies the elements of the Matrix2 by a scalar value.
		 * \param s A scalar value to multiply each Matrix2 element by.
		 * \returns A modifiable reference to the Matrix2 after the multiplication.
		 */
		Matrix2& operator*=(float s);

		/**
		 * \brief Divides the elements of the Matrix2 by a scalar value.
		 * \param s A scalar value to multiply each Matrix2 element by.
		 * \returns A modifiable reference to the Matrix2 after the division.
		 */
		Matrix2& operator/=(float s);

		/**
		 * \brief Creates a copy of the Matrix2 with all components' signs flipped.
		 * \returns A Matrix2 with all components of the original Matrix2 multiplied by -1.0f.
		 */
		Matrix2 operator-() const;
	};

	/**
	 * \brief Adds together two 2 x 2 matrices.
	 * \details Addition is performed element-wise.
	 * \param lhs A Matrix2 that will be added to the Matrix2 on the right.
	 * \param rhs A Matrix2 that will be added to the Matrix2 on the left.
	 * \returns A Matrix2 that is the sum of the left and right Matrix2 operands.
	 */
	inline Matrix2 operator+(Matrix2 lhs, const Matrix2& rhs)
	{
		return lhs += rhs;
	}

	/**
	 * \brief Subtracts one Matrix2 from another.
	 * \details Subtraction is performed element-wise.
	 * \param lhs A Matrix2 to be subtracted from.
	 * \param rhs A Matrix2 will subtracted from the left Matrix2 operand.
	 * \returns A Matrix2 that is the result of subtracting the right Matrix2 operand from the left Matrix2 operand.
	 */
	inline Matrix2 operator-(Matrix2 lhs, const Matrix2& rhs)
	{
		return lhs -= rhs;
	}

	/**
	 * \brief Multiplies the elements of a Matrix2 by a scalar value.
	 * \param lhs A Matrix2 whose elements will be multiplied by the scalar value.
	 * \param s A scalar value to multiply each Matrix2 element by.
	 * \returns A Matrix2 which is the original Matrix2 with all of its elements multiplied.
	 */
	inline Matrix2 operator*(Matrix2 lhs, float s)
	{
		return lhs *= s;
	}

	/**
	 * \brief Multiplies the elements of a Matrix2 by a scalar value.
	 * \param s A scalar value to multiply each Matrix2 element by.
	 * \param rhs A Matrix2 whose elements will be multiplied by the scalar value.
	 * \returns A Matrix2 which is the original Matrix2 with all of its elements multiplied by the scalar value.
	 */
	inline Matrix2 operator*(float s, Matrix2 rhs)
	{
		return rhs *= s;
	}

	/**
	 * \brief Divides the elements of a Matrix2 by a scalar value.
	 * \param lhs A Matrix2 whose elements will be divided by the scalar value.
	 * \param s A scalar value to divide each Matrix2 element by.
	 * \returns A Matrix2 which is the original Matrix2 with all of its elements divided by the scalar value.
	 */
	inline Matrix2 operator/(Matrix2 lhs, float s)
	{
		return lhs /= s;
	}

	/**
	 * \brief Multiplies one Matrix2 by another.
	 * \param lhs The left Matrix2 operand.
	 * \param rhs The right Matrix2 operand.
	 * \returns A Matrix2 which is the product matrix of the left Matrix2 operand and the right Matrix2 operand.
	 */
	inline Matrix2 operator*(const Matrix2& lhs, const Matrix2& rhs)
	{
		Matrix2 result;
		for (unsigned int col = 0; col < 2; col++)
			for (unsigned int row = 0; row < 2; row++)
				result[col][row] = lhs.getRow(row).dot(rhs.columns[col]);
		return result;
	}

	/**
	 * \brief Multiplies a Matrix2 with a Vector2.
	 * \param lhs The left Matrix2 operand.
	 * \param rhs The right Vector2 operand.
	 * \returns A Vector2 which is the product of the Matrix2 and the original Vector2.
	 */
	inline Vector2 operator*(const Matrix2& lhs, const Vector2& rhs)
	{
		return {
			lhs.getRow(0).dot(rhs),
			lhs.getRow(1).dot(rhs)
		};
	}

	/**
	 * \brief Checks the equivelance of two Matrix2 instances.
	 * \param lhs The left Matrix2 operand.
	 * \param rhs The right Matrix2 operand.
	 * \returns True if the elements of the two Matrix2 instances are equal.
	 */
	inline bool operator==(const Matrix2& lhs, const Matrix2& rhs)
	{
		for (unsigned int idx = 0; idx < 2; idx++)
			if (lhs[idx] != rhs[idx])
				return false;
		return true;
	}

	/**
	 * \brief Checks the not-equivelance of two Matrix2 instances.
	 * \param lhs The left Matrix2 operand.
	 * \param rhs The right Matrix2 operand.
	 * \returns True if any of the elements of the two Matrix2 instances are not equal.
	 */
	inline bool operator!=(const Matrix2& lhs, const Matrix2& rhs)
	{
		return !(lhs == rhs);
	}
}