#include "Camera.h"



Camera::Camera()
{
}


Camera::~Camera()
{
}

void Camera::Init(const Params & params)
{
	_params = params;
	BuildView();
	BuildProj();
	BuildViewProj();
}

void Camera::Shutdown()
{
}

void Camera::BuildView()
{
	const glm::vec3 target = _params.Position + _params.Direction;
	const glm::vec3 up(0.0f, 1.0f, 0.0f);
	_matV = glm::lookAtLH(_params.Position, target, up);
}

void Camera::BuildProj()
{
	SR_Assert(_params.Height > 0.0f);
	switch (_params.CamMode)
	{
	case Mode::Ortho:
	{
		const float ratio = _params.Width / _params.Height;
		const float w = _params.OrthoUnitHeight * ratio;
		const float h = _params.OrthoUnitHeight;
		_matP = glm::orthoLH
		(
			-w * 0.5f,
			w * 0.5f,
			-h * 0.5f,
			h * 0.5f,
			_params.NearPlane,
			_params.FarPlane
		);
	}
		break;
	case Mode::Perspective:
		_matP = glm::perspectiveFovLH
		(
			glm::radians(_params.PerspFov),
			_params.Width,
			_params.Height,
			_params.NearPlane,
			_params.FarPlane
		);
		break;
	default:
		SR_Assert(false);
		break;
	}
}

void Camera::BuildViewProj()
{
	_matVP = _matP * _matV;
}
