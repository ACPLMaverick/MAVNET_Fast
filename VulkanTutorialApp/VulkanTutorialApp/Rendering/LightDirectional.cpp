#include "LightDirectional.h"

#include "Camera.h"
#include "Core/HelloTriangle.h"

namespace Rendering
{
	LightDirectional::LightDirectional()
		: _color(glm::vec3(0.0f, 0.0f, 0.0f))
		, _direction(glm::vec3(0.0f, 0.0f, -1.0f))
		, _directionV(_direction)
		, _bNeedUpdateDirectionV(true)
	{
	}


	LightDirectional::~LightDirectional()
	{
	}

	void LightDirectional::Initialize(const glm::vec3 * color, const glm::vec3 * direction)
	{
		SetColor(color);
		SetDirection(direction);
	}

	void LightDirectional::Shutdown()
	{
	}

	void LightDirectional::Update()
	{
		if (Core::HelloTriangle::GetInstance()->GetCamera()->IsViewNeedUpdate())
		{
			_bNeedUpdateDirectionV = true;
		}

		if (_bNeedUpdateDirectionV)
		{
			const glm::mat4* viewMat = Core::HelloTriangle::GetInstance()->GetCamera()->GetView();
			_directionV = *viewMat * glm::vec4(_direction, 0.0f);

			_bNeedUpdateDirectionV = false;
		}
	}

	void LightDirectional::SetDirection(const glm::vec3 * direction)
	{
		_direction = *direction;
		_bNeedUpdateDirectionV = true;
	}
}