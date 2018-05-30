#include "Fog.h"

#include "HelloTriangle.h"
#include "Camera.h"

namespace Core
{
	Fog::Fog() 
		: _color(glm::vec3(1.0f, 1.0f, 1.0f))
		, _startDistance(0.0f)
		, _endDistance(std::numeric_limits<float>::max())
		, _startDepth(0.0f)
		, _endDepth(1.0f)
		, _bNeedUpdateDepthDistances(true)
	{
	}

	Fog::~Fog()
	{
	}

	void Fog::Initialize(const glm::vec3* color, float startDistance, float endDistance)
	{
		_color = *color;
		_startDistance = startDistance;
		_endDistance = endDistance;
		_bNeedUpdateDepthDistances = true;
		Update();
	}

	void Fog::Shutdown()
	{
	}

	void Fog::Update()
	{
		if (HelloTriangle::GetInstance()->GetCamera()->IsProjNeedUpdate())
		{
			_bNeedUpdateDepthDistances = true;
		}

		if (_bNeedUpdateDepthDistances)
		{
			UpdateDepthDistances();
			_bNeedUpdateDepthDistances = false;
		}
	}

	void Fog::SetStartDistance(float startDistance)
	{
		_startDistance = glm::max<float>(startDistance, 0.0f);
		_bNeedUpdateDepthDistances = true;
	}

	void Fog::SetEndDistance(float endDistance)
	{
		_endDistance = glm::max<float>(endDistance, 0.0f);
		_bNeedUpdateDepthDistances = true;
	}

	void Fog::UpdateDepthDistances()
	{
		const float nearPlane = HelloTriangle::GetInstance()->GetCamera()->GetNear();
		const float farPlane = HelloTriangle::GetInstance()->GetCamera()->GetFar();

		const float clippedNear = glm::min(glm::max(_startDistance, nearPlane), farPlane);
		const float clippedFar = glm::max(glm::min(_endDistance, farPlane), nearPlane);

		JE_Assert(clippedNear <= clippedFar);

		const float dist = farPlane - nearPlane;

		JE_Assert(dist > 0.0f);

		_startDepth = (clippedNear - nearPlane) / dist;
		_endDepth = (clippedFar - nearPlane) / dist;

		JE_Assert(_startDepth >= 0.0f && _startDepth <= 1.0f);
		JE_Assert(_endDepth >= 0.0f && _endDepth <= 1.0f);

		JE_Print("Fog: near depth: ");
		JE_Print(_startDepth);
		JE_Print(", far depth: ");
		JE_PrintLine(_endDepth);
	}
}