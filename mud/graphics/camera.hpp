#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "math/matrix/matrix_4.hpp"
#include "math/quaternion.hpp"
#include "math/vector/vector_3.hpp"

namespace mud
{
	class Camera
	{
	public:
		Camera();

		const Vector3 & getPosition() const;

		const Vector3 & getXAxis() const;

		const Vector3 & getYAxis() const;

		const Vector3 & getZAxis() const;

		const Quaternion & getOrientation() const;

		void setPosition(const Vector3 & newPosition);

		void setOrientation(const Quaternion & newOrientation);

		void move(const Vector3 & positionOffset);

		void rotate(const Quaternion & orientationOffset);

		const Matrix4 & getProjectionMatrix() const;

		const Matrix4 & getViewMatrix() const;

		Matrix4 getProjectionViewMatrix();

		void setPerspective(float verticalFOV, float aspectRatio, float zNear, float zFar);

		void setOrthographic(float width, float height, float zNear, float zFar);

		float getAspectRatio() const;

		float getZNear() const;

		float getZFar() const;

	private:
		Vector3 m_position;
		Vector3 m_forward;
		Vector3 m_right;
		Vector3 m_up;
		float m_aspectRatio;
		float m_zNear;
		float m_zFar;
		Quaternion m_orientation;
		Matrix4 m_projection;
		mutable Matrix4 m_view;
		mutable bool m_dirtyView;
	};
}

#endif