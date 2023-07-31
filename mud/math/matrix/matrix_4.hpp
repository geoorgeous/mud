/**
 * \file
 * \author \link https://georgemcdonagh.co.uk George McDonagh
 */

#pragma once

#include "../vector/vector_4.hpp"

namespace mud
{
	struct Matrix2;
	struct Matrix3;
	struct Vector3;
	struct Quaternion;

	/**
	 * \brief A mathematical 4 x 4 matrix of floating-point values.
	 * \details This struct is used to represent square matrices with 4 columns and 4 rows.
	 * \note All GML matrix data is stored in column-major order, i.e.: the contiguous memory of a matrix is stored column after column. You may often need a pointer to this data, the best way to get this is to simply use <code>&someMatrix[0][0];</code>. This provides a pointer to the matrix's first element.
	 */
	struct Matrix4
	{
		static const Matrix4 zero; //!< A static Matrix4 with all elements set to 0.0f.
		static const Matrix4 identity; //!< A static Matrix4 with all diagonal elements set to 1.0f, AKA the Identity matrix.

		Vector4 columns[4]; //!< The columns of the Matrix4

		/**
		 * \brief Default constructor. Elements of the Matrix4 are initialised to 0.0f by default.
		 */
		Matrix4() = default;

		/**
		 * \brief Constructs a Matrix4 with all diagonal elements set to a specific value.
		 * \param diagonal The value given to the diagonal elements of the Matrix4.
		 */
		Matrix4(float diagonal);

		/**
		 * \brief Constructs a Matrix4 with specified values.
		 * \param a The first column's first element. (The first row's first element)
		 * \param b The first column's second element. (The second row's first element)
		 * \param c The first column's third element. (The third row's first element)
		 * \param d The first column's fourth element. (The fourth row's first element)
		 * \param e The second column's first element. (The first row's second element)
		 * \param f The second column's second element. (The second row's second element)
		 * \param g The second column's third element. (The third row's second element)
		 * \param h The second column's fourth element. (The fourth row's second element)
		 * \param i The third column's first element. (The first row's third element)
		 * \param j The third column's second element. (The second row's third element)
		 * \param k The third column's third element. (The third row's third element)
		 * \param l The third column's fourth element. (The fourth row's third element)
		 * \param m The fourth column's first element. (The first row's fourth element)
		 * \param n The fourth column's second element. (The second row's fourth element)
		 * \param o The fourth column's third element. (The third row's fourth element)
		 * \param p The fourth column's fourth element. (The fourth row's fourth element)
		 */
		Matrix4(float a, float b, float c, float d,
			float e, float f, float g, float h,
			float i, float j, float k, float l,
			float m, float n, float o, float p);

		/**
		 * \brief Constructs a Matrix4 with specified columns.
		 * \param col0 The first column of the Matrix4.
		 * \param col1 The second column of the Matrix4.
		 * \param col2 The third column of the Matrix4.
		 * \param col3 The fourth column of the Matrix4.
		 */
		Matrix4(const Vector4& col0, const Vector4& col1, const Vector4& col2, const Vector4& col3);

		/**
		 * \brief Constructs a Matrix4 from a Matrix2.
		 * \details A Matrix2 is used to specify the elements of the top-left 2 x 2 sub-matrix of the Matrix4.
		 * \param mat2 A Matrix2 to copy elements from.
		 */
		Matrix4(const Matrix2& mat2);

		/**
		 * \brief Constructs a Matrix4 from a Matrix3.
		 * \details The Matrix3 is used to specify the elements of the top-left 3 x 3 sub-matrix of the Matrix4. The remaining
		 * \param mat3 The Matrix3 to copy elements from.
		 */
		Matrix4(const Matrix3& mat3);

		/**
		 * \brief Constructs and returns a specific row of the Matrix4.
		 * \param idx The index of the row to return.
		 * \returns A Vector4 comprised of the elements of the specified row.
		 * \warning This function performs no array-bounds checking. the passed index must be [0 alvere/math 3].
		 */
		Vector4 getRow(unsigned int idx) const;

		/**
		 * \brief Calculates and returns the transpose matrix of the Matrix4.
		 * \details The transpose of a matrix is the same matrix but with elements of rows and columns switch around. In other words the matrix rows become the columns and the columns become the rows.
		 * \returns A Matrix4 that is the transpose of the original Matrix4.
		 */
		Matrix4 transpose() const;

		/**
		 * \brief Calculates and returns the inverse matrix of the Matrix4.
		 * \details In 3D graphics, the inverse of a matrix is useful to reverse the original matrix's transformation.
		 * \returns A Matrix4 that is the inverse of the original Matrix4.
		 */
		Matrix4 inverse() const;

		/**
		 * \brief Calculates and returns the determinant of the Matrix4.
		 * \returns The determinant of the Matrix4.
		 */
		float determinant() const;

		/**
		 * \brief Accesses the columns of the Matrix4 using a given index.
		 * \param idx The specified index of the column to return.
		 * \returns A modifiable reference to the column found at the specified index.
		 * \warning This function performs no array-bounds checking. the passed index must be [0 alvere/math 3].
		 */
		Vector4& operator[](unsigned int idx);

		/**
		 * \brief Accesses the columns of the Matrix4 using a given index.
		 * \param idx The specified index of the column to return.
		 * \returns An immutable reference to the column found at the specified index.
		 * \warning This function performs no array-bounds checking. the passed index must be [0 alvere/math 3].
		 */
		const Vector4& operator[](unsigned int idx) const;

		/**
		 * \brief Adds another Matrix4 to the Matrix4.
		 * \details Addition is performed element-wise.
		 * \param rhs A Matrix4 that will be added to the Matrix4 on the left.
		 * \returns A modifiable reference to the Matrix4 after the addition.
		 */
		Matrix4& operator+=(const Matrix4& rhs);

		/**
		 * \brief Subtracts another Matrix4 from the Matrix4.
		 * \details Subtraction is performed element-wise.
		 * \param rhs A Matrix4 that will be subtracted away from the Matrix4 on the left.
		 * \returns A modifiable reference to the Matrix4 after the subtraction.
		 */
		Matrix4& operator-=(const Matrix4& rhs);

		/**
		 * \brief Multiplies the elements of the Matrix4 by a scalar value.
		 * \param s A scalar value to multiply each Matrix4 element by.
		 * \returns A modifiable reference to the Matrix4 after the multiplication.
		 */
		Matrix4& operator*=(float s);

		/**
		 * \brief Divides the elements of the Matrix4 by a scalar value.
		 * \param s A scalar value to multiply each Matrix4 element by.
		 * \returns A modifiable reference to the Matrix4 after the division.
		 */
		Matrix4& operator/=(float s);

		/**
		 * \brief Creates a copy of the Matrix4 with all components' signs flipped.
		 * \returns A Matrix4 with all components of the original Matrix4 multiplied by -1.0f.
		 */
		Matrix4 operator-() const;
	};

	/**
	 * \brief Adds together two 4 x 4 matrices.
	 * \details Addition is performed element-wise.
	 * \param lhs A Matrix4 that will be added to the Matrix4 on the right.
	 * \param rhs A Matrix4 that will be added to the Matrix4 on the left.
	 * \returns A Matrix4 that is the sum of the left and right Matrix4 operands.
	 */
	inline Matrix4 operator+(Matrix4 lhs, const Matrix4& rhs)
	{
		return lhs += rhs;
	}

	/**
	 * \brief Subtracts one Matrix4 from another.
	 * \details Subtraction is performed element-wise.
	 * \param lhs A Matrix4 to be subtracted from.
	 * \param rhs A Matrix4 will subtracted from the left Matrix4 operand.
	 * \returns A Matrix4 that is the result of subtracting the right Matrix4 operand from the left Matrix4 operand.
	 */
	inline Matrix4 operator-(Matrix4 lhs, const Matrix4& rhs)
	{
		return lhs -= rhs;
	}

	/**
	 * \brief Multiplies the elements of a Matrix4 by a scalar value.
	 * \param lhs A Matrix4 whose elements will be multiplied by the scalar value.
	 * \param s A scalar value to multiply each Matrix4 element by.
	 * \returns A Matrix4 which is the original Matrix4 with all of its elements multiplied.
	 */
	inline Matrix4 operator*(Matrix4 lhs, float s)
	{
		return lhs *= s;
	}

	/**
	 * \brief Multiplies the elements of a Matrix4 by a scalar value.
	 * \param s A scalar value to multiply each Matrix4 element by.
	 * \param rhs A Matrix4 whose elements will be multiplied by the scalar value.
	 * \returns A Matrix4 which is the original Matrix4 with all of its elements multiplied by the scalar value.
	 */
	inline Matrix4 operator*(float s, Matrix4 rhs)
	{
		return rhs *= s;
	}

	/**
	 * \brief Divides the elements of a Matrix4 by a scalar value.
	 * \param lhs A Matrix4 whose elements will be divided by the scalar value.
	 * \param s A scalar value to divide each Matrix4 element by.
	 * \returns A Matrix4 which is the original Matrix4 with all of its elements divided by the scalar value.
	 */
	inline Matrix4 operator/(Matrix4 lhs, float s)
	{
		return lhs /= s;
	}

	/**
	 * \brief Multiplies one Matrix4 by another.
	 * \param lhs The left Matrix4 operand.
	 * \param rhs The right Matrix4 operand.
	 * \returns A Matrix4 which is the product matrix of the left Matrix4 operand and the right Matrix4 operand.
	 */
	inline Matrix4 operator*(const Matrix4& lhs, const Matrix4& rhs)
	{
		Matrix4 result;
		for (unsigned int col = 0; col < 4; col++)
			for (unsigned int row = 0; row < 4; row++)
				result[col][row] = lhs.getRow(row).dot(rhs.columns[col]);
		return result;
	}

	/**
	 * \brief Multiplies a Matrix4 with a Vector4.
	 * \param lhs The left Matrix4 operand.
	 * \param rhs The right Vector4 operand.
	 * \returns A Vector4 which is the product of the Matrix4 and the original Vector4.
	 */
	inline Vector4 operator*(const Matrix4& lhs, const Vector4& rhs)
	{
		return {
			rhs.dot(lhs.getRow(0)),
			rhs.dot(lhs.getRow(1)),
			rhs.dot(lhs.getRow(2)),
			rhs.dot(lhs.getRow(3))
		};
	}

	/**
	 * \brief Checks the equivelance of two Matrix4 instances.
	 * \param lhs The left Matrix4 operand.
	 * \param rhs The right Matrix4 operand.
	 * \returns True if the elements of the two Matrix4 instances are equal.
	 */
	inline bool operator==(const Matrix4& lhs, const Matrix4& rhs)
	{
		for (unsigned int idx = 0; idx < 4; idx++)
			if (lhs[idx] != rhs[idx])
				return false;
		return true;
	}

	/**
	 * \brief Checks the not-equivelance of two Matrix4 instances.
	 * \param lhs The left Matrix4 operand.
	 * \param rhs The right Matrix4 operand.
	 * \returns True if any of the elements of the two Matrix4 instances are not equal.
	 */
	inline bool operator!=(const Matrix4& lhs, const Matrix4& rhs)
	{
		return !(lhs == rhs);
	}
}