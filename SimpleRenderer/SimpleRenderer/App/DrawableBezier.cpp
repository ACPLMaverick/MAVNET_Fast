#include "DrawableBezier.h"



DrawableBezier::DrawableBezier() : DrawableSingleObject()
{
}


DrawableBezier::~DrawableBezier()
{
}

void DrawableBezier::Init(Drawable::Params* params)
{
	// Precompute blending points.

	const float step = 0.25f;
	for (float t = 0.0f; t <= 1.005f; t += step)
	{
		BlendParam bParam(t);
		_blendParams.push_back(bParam);
	}

	DrawableSingleObject::Init(params);
}

void DrawableBezier::Draw() const
{
	glLineWidth(((Params*)_params)->Width);
	DrawableSingleObject::Draw();
}

glm::vec3 DrawableBezier::InterpolateBezier(const Bezier & bezier, const BlendParam & blendParam)
{
	return
		bezier.P0 * blendParam.B0
		+ bezier.P1 * blendParam.B1
		+ bezier.P2 * blendParam.B2
		+ bezier.P3 * blendParam.B3;
}

void DrawableBezier::CreateVertices()
{
	Params* params = (Params*)_params;
	SR_Assert(params->Points.size() > 2);

	static const float ONE_THIRD = 1.0f / 3.0f;
	static const float TWO_THIRDS = 2.0f / 3.0f;

	std::vector<glm::vec3> splitPoints;
	// Split dem points first so the curves are not so curvy.
	for (size_t i = 1; i < params->Points.size(); ++i)
	{
		const glm::vec3& start = params->Points[i - 1];
		const glm::vec3& end = params->Points[i];

		// Split into three parts.
		splitPoints.push_back(start);
		splitPoints.push_back((start + end) * 0.5f);
		//splitPoints.push_back(start * TWO_THIRDS + end * ONE_THIRD);
		//splitPoints.push_back(start * ONE_THIRD + end * TWO_THIRDS);

		if (i == params->Points.size() - 1)
		{
			splitPoints.push_back(end);
		}
	}

	// Build unique bezier for each control points.
	for (size_t i = 1; i < splitPoints.size(); ++i)
	{
		const glm::vec3& B0 = splitPoints[i - 1];
		const glm::vec3& B1 = splitPoints[i];

		Bezier bezier;

		bezier.P1 = B0 * TWO_THIRDS + B1 * ONE_THIRD;
		bezier.P2 = B0 * ONE_THIRD + B1 * TWO_THIRDS;

		if (i == 1)
		{
			bezier.P0 = B0;
		}
		else
		{
			const Bezier& prevBezier = _beziers[i - 2];	// Get previous bezier.
			bezier.P0 = prevBezier.P3;
		}

		if (i == splitPoints.size() - 1)
		{
			bezier.P3 = B1;
		}
		else
		{
			const glm::vec3& B2 = splitPoints[i + 1];
			const glm::vec3 mid13Next = B1 * TWO_THIRDS + B2 * ONE_THIRD;
			bezier.P3 = (bezier.P2 + mid13Next) * 0.5f;
		}

		_beziers.push_back(bezier);
	}

	// Compute interpolated points for this bezier in order to draw it.
	for (const Bezier& bezier : _beziers)
	{
		for (const BlendParam& blend : _blendParams)
		{
			_interpolatedPoints.push_back(InterpolateBezier(bezier, blend));
		}
	}

	if (params->DrwMode == Drawable::DrawMode::Lines)
	{
		// Make lines from interpolated points.
		for (size_t i = 0; i < _interpolatedPoints.size() - 1; ++i)
		{
			_verticesCpu.push_back({ _interpolatedPoints[i] });
			_verticesCpu.push_back({ _interpolatedPoints[i + 1] });
		}
	}
	else
	{
		for (size_t i = 0; i < _interpolatedPoints.size(); ++i)
		{
			_verticesCpu.push_back({ _interpolatedPoints[i] });
		}
	}
}
