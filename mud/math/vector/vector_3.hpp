/**
 * \file
 * \author \link https://georgemcdonagh.co.uk George McDonagh
 */

#pragma once

namespace mud
{
	struct Vector2;
	struct Vector4;
	struct Quaternion;

	/**
	 * \brief A mathematical 3D floating-point vector.
	 * \details A struct representing a 3D Vector whose components are stored as floats.
	 */
	struct Vector3
	{
		static const Vector3 zero; //!< A static Vector3 with x, y, and z members set to 0.0f.
		static const Vector3 one; //!< A static Vector3 with x, y, and z members set to 1.0f. The magnitude of this Vector3 will be 1.0f; hence a unit vector.
		static const Vector3 posX; //!< A static Vector3 with x set to 1.0f and y and z set to 0.0f.
		static const Vector3 negX; //!< A static Vector3 with x set to -1.0f and y and z set to 0.0f.
		static const Vector3 posY; //!< A static Vector3 with y set to 1.0f and x and z set to 0.0f.
		static const Vector3 negY; //!< A static Vector3 with y set to -1.0f and x and z set to 0.0f.
		static const Vector3 posZ; //!< A static Vector3 with z set to 1.0f and x and y set to 0.0f.
		static const Vector3 negZ; //!< A static Vector3 with z set to -1.0f and x and y set to 0.0f.

		float x = 0.0f; //!< The first component of the Vector3. \note This component can also be accessed via aVector3[0].
		float y = 0.0f; //!< The second component of the Vector3. \note This component can also be accessed via aVector3[1].
		float z = 0.0f; //!< The third component of the Vector3. \note This component can also be accessed via aVector3[2].

		/**
		 * \brief Default constructor. Elements of the Vector3 are initialised to 0.0f by default.
		 */
		Vector3() = default;

		/**
		 * \brief Constructor which sets all of the Vector3 components to the same value.
		 * \param f The value to give the x, y, and z components of the Vector3.
		 */
		Vector3(float f);

		/**
		 * \brief Constructs a Vector3 with individually specified values.
		 * \param x The value to give the x component.
		 * \param y The value to give the y component.
		 * \param z The value to give the z component.
		 */
		Vector3(float x, float y, float z);

		/**
		 * \brief Constructs a Vector3 using the first two components of a Vector2. The z value of the Vector3 will be set to 0.0f.
		 * \param vec2 The Vector2 from which to take the x and y component values from.
		 */
		Vector3(const Vector2& vec2);

		/**
		 * \brief Constructs a Vector3 using the first three components of a Vector4.
		 * \param vec4 The Vector4 from which to take the x, y, and z component values from.
		 */
		Vector3(const Vector4& vec4);

		/**
		 * \brief Calculates and returns the magnitude of the vector multiplied by itself.
		 * \details Useful for when performance is paramount and the user wishes to avoid the use of the sqrt() function used in magnitude().
		 * \returns The magnitude of the vector squared.
		 */
		float magnitudeSq() const;

		/**
		 * \brief Calculates and returns the magnitude of the vector.
		 * \details The vector's magnitude is also known as the vector's length or size.
		 * \returns The magnitude of the vector.
		 */
		float magnitude() const;

		/**
		 * \brief Calculates and returns the dot product of this vector and another.
		 * \param v The other vector used in calculating the dot product.
		 * \returns The dot product calculated from this vector and another.
		 */
		float dot(const Vector3& v) const;

		/**
		 * \brief Calculates and returns the distance between this vector and another.
		 * \param v The other vector.
		 * \returns The distance between the two vectors in 3D space.
		 */
		float distance(const Vector3& v) const;

		/**
		 * \brief Calculates and returns the angle between this Vector3 and another Vector3.
		 * \param v The other Vector3.
		 * \returns The angle between this Vector3 and the other.
		 * \note The angle returned by this function is in radians.
		 */
		float angle(const Vector3& v) const;

		/**
		 * \brief Calculates and returns the cross product of this Vector3 and another Vector3.
		 * \param v The other Vector3.
		 * \returns The cross product between this Vector3 and the other.
		 * \note The cross product vector will be a Vector3 exactly perpendicular to both vectors.
		*/
		Vector3 cross(const Vector3& v) const;

		/**
		 * \brief Calculates and returns a unit vector with the same direction as this vector, i.e: this vector's normal.
		 * \returns A unit vector with a magnitude of 1.
		 */
		Vector3 normal() const;

		/**
		 * \brief Normalises the vector so it has a magnitude of 1.
		 * \returns A reference to the now normalised vector.
		 */
		Vector3& normalise();

		/**
		 * \brief Limits the vector's magnitude.
		 * \details This will make the vector keep its direction but extend/shrink its magnitude.
		 * \param newMagnitude The vector's new magnitude
		 */
		Vector3& limit(float newMagnitude);

		/**
		 * \brief Accesses the vector's components using a given index.
		 * \param idx The specified index of the component to return.
		 * \returns A modifiable reference to the component found at the specified index.
		 * \warning This function performs no array-bounds checking. the passed index must be [0 alvere/math 2].
		 */
		float& operator[](unsigned int idx);

		/**
		 * \brief Accesses the vector's components using a given index.
		 * \param idx The specified index of the component to return.
		 * \returns An immutable reference to the component found at the specified index.
		 * \warning This function performs no array-bounds checking. the passed index must be [0 alvere/math 2].
		 */
		const float& operator[](unsigned int idx) const;

		/**
		 * \brief Vector3 assignment operator overload.
		 * \param rhs The Vector3 from which to copy component values from.
		 */
		Vector3& operator=(const Vector3& rhs);

		/**
		 * \brief Adds another vector to the vector.
		 * \details Addition is performed component-wise.
		 * \param rhs Another vector which will be added to the vector.
		 * \returns A reference to the vector.
		 */
		Vector3& operator+=(const Vector3& rhs);

		/**
		 * \brief Subtracts another vector from the vector.
		 * \details Subtraction is performed component-wise.
		 * \param rhs Another vector which will be subtracted from the vector.
		 * \returns A reference to the vector.
		 */
		Vector3& operator-=(const Vector3& rhs);

		/**
		 * \brief Multiplies another vector with the vector.
		 * \details Multiplication is performed component-wise.
		 * \param rhs Another vector which will be multiplied with the vector.
		 * \returns A reference to the vector.
		 */
		Vector3& operator*=(const Vector3& rhs);

		/**
		 * \brief Divides the vector by another vector.
		 * \details Division is performed component-wise.
		 * \param rhs Another vector which the vector will be divided by.
		 * \returns A reference to the vector.
		 */
		Vector3& operator/=(const Vector3& rhs);

		/**
		 * \brief Multiplies the vector by a scalar value.
		 * \param s The value of which to multiply/scale the vector by.
		 * \returns A reference to the vector.
		 */
		Vector3& operator*=(float s);

		/**
		 * \brief Divides the vector by a scalar value.
		 * \param s The value of which to divide/scale the vector by.
		 * \returns A reference to the vector.
		 */
		Vector3& operator/=(float s);

		Vector3& operator*=(const Quaternion& rhs);

		/**
		 * \brief Computes and returns the inverse of the vector, i.e: all components multiplied by -1.
		 * \returns A new vector which is the inverse of the vector.
		 */
		Vector3 operator-() const;
	};

	/**
	 * \brief Calculates the addition of two vectors.
	 * \details Addition is performed component-wise.
	 * \param lhs The vector on the left-hand-side of the addition operand.
	 * \param rhs The vector on the right-hand-side of the addition operand.
	 * \returns A new vector which is the sum of the two given vectors.
	 */
	inline Vector3 operator+(Vector3 lhs, const Vector3& rhs)
	{
		return lhs += rhs;
	}

	/**
	 * \brief Calculates the subtraction of two vectors.
	 * \details Subtraction is performed component-wise.
	 * \param lhs The vector on the left-hand-side of the subtraction operand.
	 * \param rhs The vector on the right-hand-side of the subtraction operand.
	 * \returns A new vector which is the product of the subtraction of the two given vectors.
	 */
	inline Vector3 operator-(Vector3 lhs, const Vector3& rhs)
	{
		return lhs -= rhs;
	}

	/**
	 * \brief Calculates the multiplication of two vectors.
	 * \details Multiplication is performed component-wise.
	 * \param lhs The vector on the left-hand-side of the multiplication operand.
	 * \param rhs The vector on the right-hand-side of the multiplication operand.
	 * \returns A new vector which is the product of the two given vectors.
	 */
	inline Vector3 operator*(Vector3 lhs, const Vector3& rhs)
	{
		return lhs *= rhs;
	}

	/**
	 * \brief Calculates the quotient of two vectors.
	 * \details Division is performed component-wise.
	 * \param lhs The vector on the left-hand-side of the division operand.
	 * \param rhs The vector on the right-hand-side of the division operand.
	 * \returns A new vector which is the quotient of the two given vectors.
	 */
	inline Vector3 operator/(Vector3 lhs, const Vector3& rhs)
	{
		return lhs /= rhs;
	}

	/**
	 * \brief Multiplies/scales a vector by a scalar value.
	 * \param lhs The vector to be multiplied/scaled.
	 * \param s The value to multiply/scale the vector by.
	 * \returns The result of the multiplication of the vector and the scalar value.
	 */
	inline Vector3 operator*(Vector3 lhs, float s)
	{
		return lhs *= s;
	}

	/**
	 * \brief Divides/inversely scales a vector by a value.
	 * \param lhs The vector to be divided/inversely scaled.
	 * \param s The value to divide/inversely scale the vector by.
	 * \returns The result of the division of the vector and the scalar value.
	 */
	inline Vector3 operator/(Vector3 lhs, float s)
	{
		return lhs /= s;
	}

	/**
	 * \brief Multiplies/scales a vector by a scalar value.
	 * \param s The value to multiply/scale the vector by.
	 * \param lhs The vector to be multiplied/scaled.
	 * \returns The result of the multiplication of the vector and the scalar value.
	 */
	inline Vector3 operator*(float s, Vector3 rhs)
	{
		return rhs *= s;
	}

	/**
	 * \brief Divides/inversely scales a vector by a value.
	 * \param s The value to divide/inversely scale the vector by.
	 * \param lhs The vector to be divided/inversely scaled.
	 * \returns The result of the division of the vector and the scalar value.
	 */
	inline Vector3 operator/(float s, Vector3 rhs)
	{
		return rhs /= s;
	}

	/**
	 * \brief Compares two vectors.
	 * \details Comparisons are performed per-component.
	 * \param lhs The Vector3 to be compared to the second.
	 * \param lhs The Vector3 to be compared to the first.
	 * \returns True if the vectors are found to be equal.
	 */
	inline bool operator==(const Vector3& lhs, const Vector3& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

	/**
	 * \brief Compares two vectors.
	 * \details Comparisons are performed per-component.
	 * \param lhs The Vector3 to be compared to the second.
	 * \param lhs The Vector3 to be compared to the first.
	 * \returns True if the vectors are not found to be equal.
	 */
	inline bool operator!=(const Vector3& lhs, const Vector3& rhs)
	{
		return !(lhs == rhs);
	}
}