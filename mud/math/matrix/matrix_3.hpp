/**
 * \file
 * \author \link https://georgemcdonagh.co.uk George McDonagh
 */

#pragma once

#include "../vector/vector_3.hpp"

namespace mud
{
	struct Matrix2;
	struct Matrix4;

	/**
	 * \brief A mathematical 3 x 3 matrix of floating-point values.
	 * \details This struct is used to represent square matrices with 3 columns and 3 rows.
	 * \note All GML matrix data is stored in column-major order, i.e.: the contiguous memory of a matrix is stored column after column. You may often need a pointer to this data, the best way to get this is to simply use <code>&someMatrix[0][0];</code>. This provides a pointer to the matrix's first element.
	 */
	struct Matrix3
	{
		static const Matrix3 zero; //!< A static Matrix3 with all elements set to 0.0f.
		static const Matrix3 identity; //!< A static Matrix3 with all diagonal elements set to 1.0f, AKA the Identity matrix.

		Vector3 columns[3]; //!< The columns of the Matrix3.

		/**
		 * \brief Default constructor. Elements of the Matrix3 are initialised to 0.0f by default.
		 */
		Matrix3() = default;

		/**
		 * \brief Constructs a Matrix3 with all diagonal elements set to a specific value.
		 * \param diagonal The value given to the diagonal elements of the Matrix3.
		 */
		Matrix3(float diagonal);

		/**
		 * \brief Constructs a Matrix3 with specified values.
		 * \param a The first column's first element. (The first row's first element)
		 * \param b The first column's second element. (The second row's first element)
		 * \param c The first column's third element. (The third row's first element)
		 * \param d The second column's first element. (The first row's second element)
		 * \param e The second column's second element. (The second row's second element)
		 * \param f The second column's third element. (The third row's second element)
		 * \param g The third column's first element. (The first row's third element)
		 * \param h The third column's second element. (The second row's third element)
		 * \param i The third column's third element. (The third row's third element)
		 */
		Matrix3(float a, float b, float c,
			float d, float e, float f,
			float g, float h, float i);

		/**
		 * \brief Constructs a Matrix3 with specified columns.
		 * \param col0 The first column of the Matrix3.
		 * \param col1 The second column of the Matrix3.
		 * \param col2 The third column of the Matrix3.
		 */
		Matrix3(const Vector3& col0, const Vector3& col1, const Vector3& col2);

		/**
		 * \brief Constructs a Matrix3 from a Matrix2.
		 * \details A Matrix2 is used to specify the elements of the top-left 2 x 2 sub-matrix of the Matrix3.
		 * \param mat2 A Matrix2 to copy elements from.
		 */
		Matrix3(const Matrix2& mat2);

		/**
		 * \brief Constructs a Matrix3 from a Matrix4.
		 * \details The top-left 3 x 3 sub-matrix of the Matrix4 is used to set the elements of the Matrix3.
		 * \param mat4 The Matrix4 to copy elements from.
		 */
		Matrix3(const Matrix4& mat4);

		/**
		 * \brief Constructs and returns a specific row of the Matrix3.
		 * \param idx The index of the row to return.
		 * \returns A Vector3 comprised of the elements of the specified row.
		 * \warning This function performs no array-bounds checking. the passed index must be [0 alvere/math 2].
		 */
		Vector3 getRow(unsigned int idx) const;

		/**
		 * \brief Calculates and returns the transpose matrix of the Matrix3.
		 * \details The transpose of a matrix is the same matrix but with elements of rows and columns switch around. In other words the matrix rows become the columns and the columns become the rows.
		 * \returns A Matrix3 that is the transpose of the original Matrix3.
		 */
		Matrix3 transpose() const;

		/**
		 * \brief Calculates and returns the inverse matrix of the Matrix3.
		 * \details In 3D graphics, the inverse of a matrix is useful to reverse the original matrix's transformation.
		 * \returns A Matrix3 that is the inverse of the original Matrix3.
		 */
		Matrix3 inverse() const;

		/**
		 * \brief Calculates and returns the determinant of the Matrix3.
		 * \returns The determinant of the Matrix3.
		 */
		float determinant() const;

		/**
		 * \brief Accesses the columns of the Matrix3 using a given index.
		 * \param idx The specified index of the column to return.
		 * \returns A modifiable reference to the column found at the specified index.
		 * \warning This function performs no array-bounds checking. the passed index must be [0 alvere/math 2].
		 */
		Vector3& operator[](unsigned int idx);

		/**
		 * \brief Accesses the columns of the Matrix3 using a given index.
		 * \param idx The specified index of the column to return.
		 * \returns An immutable reference to the column found at the specified index.
		 * \warning This function performs no array-bounds checking. the passed index must be [0 alvere/math 2].
		 */
		const Vector3& operator[](unsigned int idx) const;

		/**
		 * \brief Adds another Matrix3 to the Matrix3.
		 * \details Addition is performed element-wise.
		 * \param rhs A Matrix3 that will be added to the Matrix3 on the left.
		 * \returns A modifiable reference to the Matrix3 after the addition.
		 */
		Matrix3& operator+=(const Matrix3& rhs);

		/**
		 * \brief Subtracts another Matrix3 from the Matrix3.
		 * \details Subtraction is performed element-wise.
		 * \param rhs A Matrix3 that will be subtracted away from the Matrix3 on the left.
		 * \returns A modifiable reference to the Matrix3 after the subtraction.
		 */
		Matrix3& operator-=(const Matrix3& rhs);

		/**
		 * \brief Multiplies the elements of the Matrix3 by a scalar value.
		 * \param s A scalar value to multiply each Matrix3 element by.
		 * \returns A modifiable reference to the Matrix3 after the multiplication.
		 */
		Matrix3& operator*=(float s);

		/**
		 * \brief Divides the elements of the Matrix3 by a scalar value.
		 * \param s A scalar value to multiply each Matrix3 element by.
		 * \returns A modifiable reference to the Matrix3 after the division.
		 */
		Matrix3& operator/=(float s);

		/**
		 * \brief Creates a copy of the Matrix3 with all components' signs flipped.
		 * \returns A Matrix3 with all components of the original Matrix3 multiplied by -1.0f.
		 */
		Matrix3 operator-() const;
	};

	/**
	 * \brief Adds together two 3 x 3 matrices.
	 * \details Addition is performed element-wise.
	 * \param lhs A Matrix3 that will be added to the Matrix3 on the right.
	 * \param rhs A Matrix3 that will be added to the Matrix3 on the left.
	 * \returns A Matrix3 that is the sum of the left and right Matrix3 operands.
	 */
	inline Matrix3 operator+(Matrix3 lhs, const Matrix3& rhs)
	{
		return lhs += rhs;
	}

	/**
	 * \brief Subtracts one Matrix3 from another.
	 * \details Subtraction is performed element-wise.
	 * \param lhs A Matrix3 to be subtracted from.
	 * \param rhs A Matrix3 will subtracted from the left Matrix3 operand.
	 * \returns A Matrix3 that is the result of subtracting the right Matrix3 operand from the left Matrix3 operand.
	 */
	inline Matrix3 operator-(Matrix3 lhs, const Matrix3& rhs)
	{
		return lhs -= rhs;
	}

	/**
	 * \brief Multiplies the elements of a Matrix3 by a scalar value.
	 * \param lhs A Matrix3 whose elements will be multiplied by the scalar value.
	 * \param s A scalar value to multiply each Matrix3 element by.
	 * \returns A Matrix3 which is the original Matrix3 with all of its elements multiplied.
	 */
	inline Matrix3 operator*(Matrix3 lhs, float s)
	{
		return lhs *= s;
	}

	/**
	 * \brief Multiplies the elements of a Matrix3 by a scalar value.
	 * \param s A scalar value to multiply each Matrix3 element by.
	 * \param rhs A Matrix3 whose elements will be multiplied by the scalar value.
	 * \returns A Matrix3 which is the original Matrix3 with all of its elements multiplied by the scalar value.
	 */
	inline Matrix3 operator*(float s, Matrix3 rhs)
	{
		return rhs *= s;
	}

	/**
	 * \brief Divides the elements of a Matrix3 by a scalar value.
	 * \param lhs A Matrix3 whose elements will be divided by the scalar value.
	 * \param s A scalar value to divide each Matrix3 element by.
	 * \returns A Matrix3 which is the original Matrix3 with all of its elements divided by the scalar value.
	 */
	inline Matrix3 operator/(Matrix3 lhs, float s)
	{
		return lhs /= s;
	}

	/**
	 * \brief Multiplies one Matrix3 by another.
	 * \param lhs The left Matrix3 operand.
	 * \param rhs The right Matrix3 operand.
	 * \returns A Matrix3 which is the product matrix of the left Matrix3 operand and the right Matrix3 operand.
	 */
	inline Matrix3 operator*(const Matrix3& lhs, const Matrix3& rhs)
	{
		Matrix3 result;
		for (unsigned int col = 0; col < 3; col++)
			for (unsigned int row = 0; row < 3; row++)
				result[col][row] = lhs.getRow(row).dot(rhs.columns[col]);
		return result;
	}

	/**
	 * \brief Multiplies a Matrix3 with a Vector3.
	 * \param lhs The left Matrix3 operand.
	 * \param rhs The right Vector3 operand.
	 * \returns A Vector3 which is the product of the Matrix3 and the original Vector3.
	 */
	inline Vector3 operator*(const Matrix3& lhs, const Vector3& rhs)
	{
		return {
			lhs.getRow(0).dot(rhs),
			lhs.getRow(1).dot(rhs),
			lhs.getRow(2).dot(rhs)
		};
	}

	/**
	 * \brief Checks the equivelance of two Matrix3 instances.
	 * \param lhs The left Matrix3 operand.
	 * \param rhs The right Matrix3 operand.
	 * \returns True if the elements of the two Matrix3 instances are equal.
	 */
	inline bool operator==(const Matrix3& lhs, const Matrix3& rhs)
	{
		for (unsigned int idx = 0; idx < 3; idx++)
			if (lhs[idx] != rhs[idx])
				return false;
		return true;
	}

	/**
	 * \brief Checks the not-equivelance of two Matrix3 instances.
	 * \param lhs The left Matrix3 operand.
	 * \param rhs The right Matrix3 operand.
	 * \returns True if any of the elements of the two Matrix3 instances are not equal.
	 */
	inline bool operator!=(const Matrix3& lhs, const Matrix3& rhs)
	{
		return !(lhs == rhs);
	}
}