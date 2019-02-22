#include "Camera.h"


namespace Rendering
{
	const float Camera::ARBITRARY_DIRECTION_TARGET_DISTANCE = 10.0f;

	Camera::Camera()
		: _view(glm::mat4(1.0f))
		, _proj(glm::mat4(1.0f))
		, _viewProj(glm::mat4(1.0f))
		, _position(glm::vec3(0.0f, -1.0f, 0.0f))
		, _target(glm::vec3(0.0f, 0.0f, 0.0f))
		, _direction(glm::vec3(0.0f, 1.0f, 0.0f))
		, _dimension(0.0f)
		, _near(0.0f)
		, _far(0.0f)
		, _bUpdateView(false)
		, _bUpdateProj(false)
	{
	}


	Camera::~Camera()
	{
	}

	void Camera::Initialize
	(
		const glm::vec3 * position, 
		const glm::vec3 * target, 
		float fov,
		float dimension, 
		float nearPlane, 
		float farPlane
	)
	{
		_position = *position;
		_target = *target;
		CalcDirection();
		_fov = fov;
		_dimension = dimension;
		_near = nearPlane;
		_far = farPlane;

		JE_Assert(_near < _far);

		_bUpdateView = true;
		_bUpdateProj = true;

		Update();
	}

	void Camera::Shutdown()
	{
		// Do nothing.
	}

	void Camera::Update()
	{
		if (_bUpdateView)
		{
			UpdateView();
		}

		if (_bUpdateProj)
		{
			UpdateProj();
		}

		if (_bUpdateView || _bUpdateProj)
		{
			UpdateViewProj();
		}

		_bUpdateView = false;
		_bUpdateProj = false;
	}

	void Camera::SetPosition(const glm::vec3 * position)
	{
		const glm::vec3 diff = *position - _position;
		_position = *position;
		_target += diff;
		// Direction doesn't change here.
		_bUpdateView = true;
	}

	void Camera::SetPositionKeepTarget(const glm::vec3 * position)
	{
		_position = *position;
		_bUpdateView = true;
	}

	void Camera::SetTarget(const glm::vec3 * target)
	{
		_target = *target;
		CalcDirection();
		_bUpdateView = true;
	}

	void Camera::SetDirection(const glm::vec3 * direction)
	{
		_direction = *direction;
		CalcTarget();
		_bUpdateView = true;
	}

	void Camera::SetFov(float fov)
	{
		_fov = fov;
		_bUpdateProj = true;
	}

	void Camera::SetDimension(float dimension)
	{
		_dimension = dimension;
		_bUpdateProj = true;
	}

	void Camera::SetNear(float nearPlane)
	{
		JE_Assert(nearPlane < _far);
		_near = nearPlane;
		_bUpdateProj = true;
	}

	void Camera::SetFar(float farPlane)
	{
		JE_Assert(farPlane > _near);
		_far = farPlane;
		_bUpdateProj = true;
	}

	void Camera::CalcDirection()
	{
		_direction = _target - _position;
		_direction = glm::normalize(_direction);
	}

	void Camera::CalcTarget()
	{
		_target = _position + _direction * ARBITRARY_DIRECTION_TARGET_DISTANCE;
	}

	void Camera::UpdateView()
	{
		_view = glm::lookAt(_position, _target, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void Camera::UpdateProj()
	{
		_proj = glm::perspective(glm::radians(_fov), _dimension, _near, _far);
		_proj[1][1] *= -1.0f; //GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted.The easiest way to compensate for that is to flip the sign on the scaling factor of the Y axis in the projection matrix.
	}

	void Camera::UpdateViewProj()
	{
		_viewProj = _proj * _view;
	}
}