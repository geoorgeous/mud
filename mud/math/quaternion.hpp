/**
 * \file
 * \author \link https://georgemcdonagh.co.uk George McDonagh
 */

#pragma once

#include "vector/vector_3.hpp"

namespace mud
{
	/**
	 * \brief A quaternion struct used to represent 3D rotations.
	 */
	struct Quaternion
	{
		static Quaternion identity; //!< A static Quaternion with all components set to 0.0f, except from w, which is set to 1.0f.

		float w = 0.0f; //!< The w component of the Quaternion.
		float x = 0.0f; //!< The x component of the Quaternion.
		float y = 0.0f; //!< The y component of the Quaternion.
		float z = 0.0f; //!< The z component of the Quaternion.

		/**
		 * \brief Constructs a Quaternion from a specific set of Euler angles.
		 * \param eulerAngles A Vector3 with X, Y, and Z Euler angles in radians.
		 * \returns A quaternion rotation.
		 */
		static Quaternion fromEulerAngles(const Vector3& eulerAngles);

		/**
		 * \brief Constructs and returns a Vector3 from the x, y, and z components of the Quaternion.
		 * \returns A Vector3 instance.
		 */
		Vector3 xyz() const;

		float sqrMagnitude() const;

		float magnitude() const;

		/**
		 * \brief Constructs and returns the conjugate of the Quaternion.
		 * \returns A Quaternion instance which is the conjugate of the original Quaternion.
		 */
		Quaternion conjugate() const;

		/**
		 * \brief Constructs and returns the normalised version of the Quaternion.
		 * \returns A Quaternion instance which is the normalised version of the original Quaternion.
		 */
		Quaternion normal() const;

		/**
		 * \breif Normalises the Quaternion.
		 * \returns A reference to the Quaternion after it has been normalised.
		 */
		Quaternion& normalise();

		Quaternion inverse() const;

		/**
		 * \brief Calculates and returns the Quaternion rotation as a set of Euler angles.
		 * \returns A Vector3 whose x, y, and z components are a set of Euler angles in radians.
		 */
		Vector3 getEulerAngles() const;

		/**
		 * \brief Assigns another set of Quaternion component values to the original.
		 * \param rhs A Quaternion whose values will be assigned to the original.
		 * \returns A reference to the Quaternion after it has been assigned.
		 */
		Quaternion& operator=(const Quaternion& rhs);

		/**
		 * \brief Adds another Quaternion to the Quaternion.
		 * \param rhs A Quaternion to add to the left Quaternion operand.
		 * \returns A reference to the Quaternion after the addition is carried out.
		 */
		Quaternion& operator+=(const Quaternion& rhs);

		/**
		 * \brief Subtracts one Quaternion from the Quaternion.
		 * \param rhs A Quaternion to subtract from the left Quaternion operand.
		 * \returns A reference to the Quaternion after the subtraction is carried out.
		 */
		Quaternion& operator-=(const Quaternion& rhs);

		/**
		 * \brief Multiplies two quaternion rotations together.
		 * \details Multiplying two quaternion rotations together this way results in a Quaternion that is the combination of the two Quaternion operand's roations.
		 * \param rhs A Quaternion that will be multiplied with the left Quaternion operand.
		 * \returns A reference to the Quaternion after it has been multiplied.
		 * \warning Quaternion multiplication is not commutitive.
		 */
		Quaternion& operator*=(const Quaternion& rhs);
	};

	/**
	 * \brief Constructs a quaternion rotation around a specified axis.
	 * \param degrees The amount to rotate around the specified axis in radians.
	 * \param axis The 3D axis of rotation.
	 * \param normaliseAxis Pass true if the axis being passed is not a normalised vector.
	 * \returns A quaternion rotation.
	 */
	Quaternion axisAngle(float degrees, const Vector3& axis, bool normaliseAxis);

	/**
	 * \brief Performs component-wise addition on two Quaternion instances.
	 * \param lhs A Quaternion to add to the right Quaternion operand.
	 * \param rhs A Quaternion to add to the left Quaternion operand.
	 * \returns A Quaternion which is the sum of the left and right Quaternion operands.
	 */
	inline Quaternion operator+(Quaternion lhs, const Quaternion& rhs)
	{
		return lhs += rhs;
	}

	/**
	 * \brief Performs component-wise subtraction on two Quaternion instances.
	 * \param lhs A Quaternion to subtract from.
	 * \param rhs A Quaternion which will be subtracted from the left Quaternion operand.
	 * \returns A Quaternion which is the product of the subtraction of the two Quaternions instances.
	 */
	inline Quaternion operator-(Quaternion lhs, const Quaternion& rhs)
	{
		return lhs -= rhs;
	}

	/**
	 * \brief Multiplies two quaternion rotations together.
	 * \details Multiplying two quaternion rotations together this way results in a Quaternion that is the combination of the two Quaternion operand's roations.
	 * \param lhs A Quaternion that will be multiplied with the right Quaternion operand.
	 * \param rhs A Quaternion that will be multiplied with the left Quaternion operand.
	 * \returns A quaternion which is the product of the left and right Quaternion operands.
	 * \warning Quaternion multiplication is not commutitive.
	 */
	inline Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
	{
		return Quaternion{
			lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z,
			lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
			lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w
		};
	}

	/**
	 * \brief Multiplies a Vector3 by a Quaternion, essentially rotating the Vector3.
	 * \param lhs A Vector3 which will be rotated by a quaternion rotation.
	 * \param rhs A Quaternion which will be used to rotate the Vector3.
	 * \returns A new Vector3 which is the original vector3 rotated by the quaternion rotation.
	 */
	inline Vector3 operator*(const Vector3& lhs, const Quaternion& rhs)
	{
		return
			2.0f * rhs.xyz().dot(lhs) * rhs.xyz()
			+ (rhs.w * rhs.w - rhs.xyz().dot(rhs.xyz())) * lhs
			+ 2.0f * rhs.w * rhs.xyz().cross(lhs);
	}

	/**
	 * \brief Checks the equivelance of two Quaternion instances.
	 * \param lhs The left Quaternion operand.
	 * \param rhs The right Quaternion operand.
	 * \returns True if the components of the two Quaternion instances are equal.
	 */
	inline bool operator==(const Quaternion& lhs, const Quaternion& rhs)
	{
		return !(lhs.w != rhs.w || lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z);
	}

	/**
	 * \brief Checks the not-equivelance of two Quaternion instances.
	 * \param lhs The left Quaternion operand.
	 * \param rhs The right Quaternion operand.
	 * \returns True if any of the components of the two Quaternion instances are not equal.
	 */
	inline bool operator!=(const Quaternion& lhs, const Quaternion& rhs)
	{
		return !(lhs == rhs);
	}
}