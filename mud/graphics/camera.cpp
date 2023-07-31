#include "camera.hpp"

#include "math/constants.hpp"
#include "math/matrix/transformations.hpp"

namespace mud
{
	Camera::Camera()
		: m_position(0), m_forward(Vector3::negZ), m_right(Vector3::posX), m_up(Vector3::posY), m_zNear(0), m_zFar(0), m_orientation(Quaternion::identity), m_projection(Matrix4::identity), m_view(Matrix4::identity), m_dirtyView(false)
	{}

	const Vector3 & Camera::getPosition() const
	{
		return m_position;
	}

	const Vector3 & Camera::getXAxis() const
	{
		return m_right;
	}

	const Vector3 & Camera::getYAxis() const
	{
		return m_up;
	}

	const Vector3 & Camera::getZAxis() const
	{
		return m_forward;
	}

	const Quaternion & Camera::getOrientation() const
	{
		return m_orientation;
	}

	void Camera::setPosition(const Vector3 & newPosition)
	{
		m_position = newPosition;
		m_dirtyView = true;
	}

	void Camera::setOrientation(const Quaternion & newOrientation)
	{
		m_orientation = newOrientation;
		m_forward = Vector3::negZ * m_orientation;
		m_right = Vector3::posX * m_orientation;
		m_up = Vector3::posY * m_orientation;
		m_dirtyView = true;
	}

	void Camera::move(const Vector3 & positionOffset)
	{
		setPosition(m_position + (m_forward * positionOffset.z) + (m_right * positionOffset.x) + (m_up * positionOffset.y));
	}

	void Camera::rotate(const Quaternion & orientationOffset)
	{
		setOrientation(m_orientation * orientationOffset);
	}

	const Matrix4 & Camera::getProjectionMatrix() const
	{
		return m_projection;
	}

	const Matrix4 & Camera::getViewMatrix() const
	{
		if (m_dirtyView)
		{
			m_view = lookAt(m_position, m_position + m_forward, m_up);
			m_dirtyView = false;
		}

		return m_view;
	}

	Matrix4 Camera::getProjectionViewMatrix()
	{
		return getProjectionMatrix() * getViewMatrix();
	}

	void Camera::setPerspective(float verticalFOV, float aspectRatio, float zNear, float zFar)
	{
		m_projection = perspective(_TAU_DIV_360 * verticalFOV, aspectRatio, zNear, zFar);
		m_aspectRatio = aspectRatio;
		m_zNear = zNear;
		m_zFar = zFar;
	}

	void Camera::setOrthographic(float width, float height, float zNear, float zFar)
	{
		const float halfWidth = width * 0.5f;
		const float halfHeight = height * 0.5f;
		m_projection = orthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, zNear, zFar);
		m_aspectRatio = width / height;
		m_zNear = zNear;
		m_zFar = zFar;
	}

	float Camera::getAspectRatio() const
	{
		return m_aspectRatio;
	}

	float Camera::getZNear() const
	{
		return m_zNear;	
	}

	float Camera::getZFar() const
	{
		return m_zFar;
	}
}