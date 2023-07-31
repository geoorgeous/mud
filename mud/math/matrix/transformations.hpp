/**
 * \file
 * \author \link https://georgemcdonagh.co.uk George McDonagh
 */

#pragma once

#include "matrix_4.hpp"

namespace mud
{
	struct Quaternion;
	struct Vector3;

	/**
	 * \brief Constructs a translation transform matrix with specified X, Y, and Z translation values.
	 * \param tX The value to translate along the X axis.
	 * \param tY The value to translate along the Y axis.
	 * \param tZ The value to translate along the Z axis.
	 * \returns A Matrix4 translation transform matrix.
	 */
	Matrix4 transform_t(float tX, float tY, float tZ);

	/**
	 * \brief Constructs a translation transform matrix from the components of a Vector3.
	 * \param v A Vector3 with the X, Y, and Z values of the translation.
	 * \returns A Matrix4 translation transform matrix.
	 */
	Matrix4 transform_t(const Vector3& v);

	/**
	 * \brief Constructs a scale transform matrix with specified X, Y, and Z scale values.
	 * \param tX The value to scale in the X axis.
	 * \param tY The value to scale in the Y axis.
	 * \param tZ The value to scale in the Z axis.
	 * \returns A Matrix4 scale transform matrix.
	 */
	Matrix4 transform_s(float sX, float sY, float sZ);

	/**
	 * \brief Constructs a scale transform matrix from the components of a Vector3.
	 * \param v A Vector3 with the X, Y, and Z values of the scale.
	 * \returns A Matrix4 scale transform matrix.
	 */
	Matrix4 transform_s(const Vector3& v);

	/**
	 * \brief Constructs a rotation transform matrix from a Quaternion rotation.
	 * \param q A Quaternion which represents a rotation in 3D space.
	 * \returns A Matrix4 rotation transform matrix.
	 */
	Matrix4 transform_r(const Quaternion& q);

	/**
	 * \brief Constructs a rotation transform matrix around the X axis.
	 * \param radians The angle the transform matrix will rotate around the X axis in radians.
	 * \returns A Matrix4 rotation transform matrix.
	 */
	Matrix4 transform_rX(float radians);

	/**
	 * \brief Constructs a rotation transform matrix around the Y axis.
	 * \param radians The angle the transform matrix will rotate around the Y axis in radians.
	 * \returns A Matrix4 rotation transform matrix.
	 */
	Matrix4 transform_rY(float radians);

	/**
	 * \brief Constructs a rotation transform matrix around the Z axis.
	 * \param radians The angle the transform matrix will rotate around the Z axis in radians.
	 * \returns A Matrix4 rotation transform matrix.
	 */
	Matrix4 transform_rZ(float radians);

	void transformDecompose(Matrix4 t, Vector3 & translation, Vector3 & scale, Vector3 & rotation);

	/**
	 * \brief Constructs a perspective projection matrix.
	 * \param verticalFOV The vertical angle of the perspective projection's frustrum in radians.
	 * \param aspect The aspect ratio ofthe perspective projection's frustrum.
	 * \param zNear The distance of the near plane of the perpective projection's frustrum from the camera.
	 * \param zFar The distance of the far plane of the perpective projection's frustrum from the camera.
	 */
	Matrix4 perspective(float verticalFOV, float aspect, float zNear, float zFar);

	Matrix4 orthographic(float left, float right, float top, float bottom, float zNear, float zFar);

	/**
	 * \brief Constructs a view matrix.
	 * \param right The view matrix's X axis direction.
	 * \param up The view matrix's Y axis direction.
	 * \param forward The view matrix's Z axis direction.
	 * \param eye The position of the camera/view position.
	 */
	Matrix4 view(const Vector3& right, const Vector3& up, const Vector3& forward, const Vector3& eye);

	/**
	 * \brief Constructs a look-at view matrix.
	 * \param eye The position of the camera/view position.
	 * \param target The position of the target to look at.
	 * \param up The world's up direction.
	 */
	Matrix4 lookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
}