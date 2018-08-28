#include "SceneBezier.h"

#include "App.h"
#include "Shader.h"
#include "Camera.h"

#include "DrawablePoint.h"
#include "DrawableLine.h"
#include "DrawableBezier.h"

SceneBezier::SceneBezier()
	: Scene()
	, _line(nullptr)
	, _bezier(nullptr)
	, _markerLine(nullptr)
	, _markerBezier(nullptr)
{
}


SceneBezier::~SceneBezier()
{
}

void SceneBezier::InitResources()
{
	const std::string colorName("Color");
	Shader* shdColor = new Shader();
	shdColor->Init(colorName);
	_shaders.emplace(colorName, shdColor);
}

void SceneBezier::InitCamera()
{
	_camera = new Camera();
	Camera::Params params;
	int w, h;
	glfwGetWindowSize(App::GetInstance().GetWindowPtr(), &w, &h);
	params.Width = (float)w;
	params.Height = (float)h;
	_camera->Init(params);
}

void SceneBezier::InitDrawables()
{
	Shader* shdColor = _shaders["Color"];
	SR_Assert(shdColor != nullptr);

	std::vector<glm::vec3> plotPoints;
	plotPoints.push_back(glm::vec3(-8.0f, 2.0f, 0.0f));
	plotPoints.push_back(glm::vec3(-4.0f, -2.0f, 0.0f));
	plotPoints.push_back(glm::vec3(0.0f, 6.0f, 0.0f));
	plotPoints.push_back(glm::vec3(5.0f, 3.0f, 0.0f));
	plotPoints.push_back(glm::vec3(6.0f, 0.0f, 0.0f));

	
	DrawablePoint* point = new DrawablePoint();
	_drawables.push_back(point);

	DrawablePoint::Params* params = new DrawablePoint::Params();
	params->Shd = shdColor;
	params->DrwMode = Drawable::DrawMode::Points;
	params->Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	params->Scale = 20.0f;

	point->Init(params);

	_playerDrawable = point;


	DrawableLine* line = new DrawableLine();
	_drawables.push_back(line);

	DrawableLine::Params* lineParams = new DrawableLine::Params();
	lineParams->Shd = shdColor;
	lineParams->Color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	lineParams->Width = 4.0f;
	for (const glm::vec3& point : plotPoints)
		lineParams->Points.push_back(point);

	line->Init(lineParams);
	_line = line;

	/*
	DrawableBezier* bezier = new DrawableBezier();
	_drawables.push_back(bezier);

	DrawableBezier::Params* bezierParams = new DrawableBezier::Params();
	bezierParams->Shd = shdColor;
	bezierParams->Color = glm::vec4(0.3f, 0.3f, 1.0f, 1.0f);
	bezierParams->Width = 2.0f;
	bezierParams->DrwMode = Drawable::DrawMode::Lines;
	for (const glm::vec3& point : plotPoints)
		bezierParams->Points.push_back(point);

	bezier->Init(bezierParams);
	_bezier = bezier;
	*/

	point = new DrawablePoint();
	_drawables.push_back(point);

	DrawablePoint::Params* paramsMarker = new DrawablePoint::Params();
	paramsMarker->Shd = shdColor;
	paramsMarker->DrwMode = Drawable::DrawMode::Points;
	paramsMarker->Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	paramsMarker->Scale = 16.0f;

	point->Init(paramsMarker);
	_markerLine = point;

	/*
	point = new DrawablePoint();
	_drawables.push_back(point);

	DrawablePoint::Params* paramsMarkerBezier = new DrawablePoint::Params();
	paramsMarkerBezier->Shd = shdColor;
	paramsMarkerBezier->DrwMode = Drawable::DrawMode::Points;
	paramsMarkerBezier->Color = glm::vec4(1.0f, 0.8f, 0.1f, 1.0f);
	paramsMarkerBezier->Scale = 8.5f;
	paramsMarkerBezier->Position = glm::vec3(0.0f, -8.0f, 0.0f);

	point->Init(paramsMarkerBezier);
	_markerBezier = point;
	*/
}

void SceneBezier::UpdateLogic()
{
	MovePlayer();
	ComputeLineMarker();
	//ComputeBezierMarker();
}

void SceneBezier::MovePlayer()
{
	glm::vec3 moveVec(0.0f);
	if (App::GetInstance().GetInput().IsKeyDown(GLFW_KEY_UP))
	{
		moveVec.y += 1.0f;
	}
	if (App::GetInstance().GetInput().IsKeyDown(GLFW_KEY_DOWN))
	{
		moveVec.y -= 1.0f;
	}
	if (App::GetInstance().GetInput().IsKeyDown(GLFW_KEY_LEFT))
	{
		moveVec.x -= 1.0f;
	}
	if (App::GetInstance().GetInput().IsKeyDown(GLFW_KEY_RIGHT))
	{
		moveVec.x += 1.0f;
	}

	if (moveVec.x != 0.0f || moveVec.y != 0.0f)
	{
		float baseMovement = App::GetDt() * 5.0f;
		if (App::GetInstance().GetInput().IsKeyDown(GLFW_KEY_LEFT_SHIFT))
		{
			baseMovement *= 6.0f;
		}
		moveVec = glm::normalize(moveVec);
		moveVec *= baseMovement;
		moveVec.z = 0.0f;

		DrawablePoint::Params& playerParams = (DrawablePoint::Params&)_playerDrawable->GetParams();
		playerParams.Position += moveVec;
	}
}

void SceneBezier::ComputeLineMarker()
{
	const glm::vec3 eyePos = ((DrawablePoint::Params&)_playerDrawable->GetParams(false)).Position;

	glm::vec3 closestPoint, secondClosestPoint, closestVertex;
	const std::vector<glm::vec3>& points = ((DrawableLine::Params&)_line->GetParams(false)).Points;
	GetClosestPoint(points.data(), points.size(), eyePos, closestPoint, &secondClosestPoint, &closestVertex);

	const bool bEnhancedMode = true;

	if (bEnhancedMode)
	{
		const glm::vec3 diffA = eyePos - closestPoint;
		const glm::vec3 diffB = eyePos - secondClosestPoint;
		const glm::vec3 pointToVertex = closestVertex - closestPoint;
		const float pointToVertexDistSqr = glm::dot(pointToVertex, pointToVertex);

		const float distSqrA = glm::dot(diffA, diffA);
		const float distSqrB = glm::dot(diffB, diffB);

		const float DIFF_THRES = 1.0f * pointToVertexDistSqr;
		const float diff = glm::abs<float>(distSqrA - distSqrB);

		if (diff < DIFF_THRES)
		{
			// Get the intersection point of closestPoint - secondClosestPoint and eyePos - closestVertex
			glm::vec3 a1 = closestPoint;
			glm::vec3 a2 = secondClosestPoint;
			glm::vec3 a3 = eyePos;
			glm::vec3 a4 = closestVertex;

			auto cmpDet = [](float a, float b, float c, float d) -> float
			{
				return a * d - b * c;
			};

			bool isCross = true;
			float detL1 = cmpDet(a1.x, a1.y, a2.x, a2.y);
			float detL2 = cmpDet(a3.x, a3.y, a4.x, a4.y);
			float x1mx2 = a1.x - a2.x;
			float x3mx4 = a3.x - a4.x;
			float y1my2 = a1.y - a2.y;
			float y3my4 = a3.y - a4.y;

			float xnom = cmpDet(detL1, x1mx2, detL2, x3mx4);
			float ynom = cmpDet(detL1, y1my2, detL2, y3my4);
			float denom = cmpDet(x1mx2, y1my2, x3mx4, y3my4);
			if (fabsf(denom) < 0.00001f)
			{
				isCross = false;
			}

			if(isCross)
			{
				glm::vec3 pointBetween = glm::vec3(xnom / denom, ynom / denom, (a1.z + a2.z) * 0.5f);

				float lerpVal = (1.0f - (diff / DIFF_THRES));
				lerpVal = lerpVal * lerpVal;
				pointBetween = glm::lerp(closestPoint, pointBetween, lerpVal);

				closestPoint = pointBetween;
				// Calculate final diff
				/*
				{
					const glm::vec3 finalDiffs[2] = { eyePos - closestPoint, eyePos - pointBetween };
					const float finalLengths[2] = {
						glm::dot(finalDiffs[0], finalDiffs[0]), glm::dot(finalDiffs[1], finalDiffs[1]) };

					if (finalLengths[1] < finalLengths[0])
					{
						closestPoint = pointBetween;
					}
				}
				*/
			}
		}
	}

	// Apply to marker.
	closestPoint = glm::lerp(
		((DrawablePoint::Params&)_markerLine->GetParams(false)).Position,
		closestPoint, 0.1f);
	((DrawablePoint::Params&)_markerLine->GetParams()).Position = closestPoint;
}

void SceneBezier::ComputeBezierMarker()
{
	glm::vec3 closestPoint = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	float closestDistSqr = FLT_MAX;

	const glm::vec3 eyePos = ((DrawablePoint::Params&)_playerDrawable->GetParams(false)).Position;

	const std::vector<DrawableBezier::Bezier>& beziers = _bezier->GetBeziers();

	// Find two closest beziers.
	const DrawableBezier::Bezier* closestBeziers[2] = {};
	float bezierDistsSqr[2] = { FLT_MAX, FLT_MAX };
	for (const DrawableBezier::Bezier& bezier : beziers)
	{
		// Simply get bezier's midpoint.
		const glm::vec3 midpoint = (bezier.P0 + bezier.P1 + bezier.P2 + bezier.P3) * 0.25f;

		const glm::vec3 diff = eyePos - midpoint;
		const float distSqr = glm::dot(diff, diff);

		if (distSqr < bezierDistsSqr[0])
		{
			closestBeziers[1] = closestBeziers[0];
			bezierDistsSqr[1] = bezierDistsSqr[0];

			closestBeziers[0] = &bezier;
			bezierDistsSqr[0] = distSqr;
		}
		else if (distSqr < bezierDistsSqr[1] && distSqr != bezierDistsSqr[0])
		{
			closestBeziers[1] = &bezier;
			bezierDistsSqr[1] = distSqr;
		}
	}

	// Discretize their points and find the closest one using traditional method.
	const float step = 0.05f;
	std::vector<glm::vec3> discretePoints;
	SR_Assert(closestBeziers[0] != nullptr && closestBeziers[1] != nullptr && closestBeziers[0] != closestBeziers[1]);

	glm::vec3 closestPoints[2];

	for (size_t i = 0; i < 2; ++i)
	{
		discretePoints.clear();
		const DrawableBezier::Bezier* curBezier = closestBeziers[i];

		const float finalStep = i == 1 ? 1.001f : 1.001f - step;
		for (float t = 0.0f; t <= finalStep; t += step)
		{
			DrawableBezier::BlendParam blend(t);
			discretePoints.push_back(DrawableBezier::InterpolateBezier(*curBezier, blend));
		}

		// Find two closest points out of the discretized ones.
		glm::vec3 twoClosest[2] = {};
		float twoClosestDists[2] = { FLT_MAX, FLT_MAX };

		for (const glm::vec3& point : discretePoints)
		{
			const glm::vec3 diff = eyePos - point;
			const float distSqr = glm::dot(diff, diff);

			if (distSqr < twoClosestDists[0])
			{
				twoClosest[1] = twoClosest[0];
				twoClosestDists[1] = twoClosestDists[0];

				twoClosest[0] = point;
				twoClosestDists[0] = distSqr;
			}
			else if (distSqr < twoClosestDists[1] && distSqr != twoClosestDists[0])
			{
				twoClosest[1] = point;
				twoClosestDists[1] = distSqr;
			}
		}

		//GetClosestPoint(discretePoints.data(), discretePoints.size(), eyePos, closestPoints[i]);
		GetClosestPoint(twoClosest, 2, eyePos, closestPoints[i]);
	}

	const glm::vec3 finalDiffs[2] = { eyePos - closestPoints[0], eyePos - closestPoints[1] };
	const float finalLengths[2] = { 
		glm::dot(finalDiffs[0], finalDiffs[0]), glm::dot(finalDiffs[1], finalDiffs[1]) };

	if (finalLengths[0] < finalLengths[1])
	{
		closestPoint = closestPoints[0];
	}
	else
	{
		closestPoint = closestPoints[1];
	}

	// Apply to marker.
	//closestPoint = glm::lerp(
	//	((DrawablePoint::Params&)_markerBezier->GetParams(false)).Position,
	//	closestPoint, 0.1f);
	((DrawablePoint::Params&)_markerBezier->GetParams()).Position = closestPoint;
}

void SceneBezier::GetClosestPoint(const glm::vec3* lines, const size_t lineNum, const glm::vec3& source, glm::vec3 & outClosestPoint, glm::vec3* outSecondClosestPoint/* = nullptr*/, glm::vec3* outClosestLineVertex /*= nullptr*/)
{
	glm::vec3 closestPoint = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	float closestDistSqr = FLT_MAX;

	float tmpDistance;
	glm::vec3 tmpClosest;

	const size_t soundPositionsNum = lineNum;

	glm::vec3 secondClosestPoint = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	float secondClosestDistSqr = FLT_MAX;

	size_t closestIndex = soundPositionsNum;
	size_t secondClosestIndex = soundPositionsNum;

	// Line strip.
	for (size_t i = 0; i < (soundPositionsNum - 1); i++)
	{
		assert(i < soundPositionsNum && (i + 1) < soundPositionsNum);

		glm::vec3 lineA = lines[i];
		glm::vec3 lineB = lines[i + 1];

		// Project point onto a line.
		glm::vec3 posToLineA = source - lineA;
		glm::vec3 lineAToB = lineB - lineA;


		//float mplier = sgvector_project3(lineAToB, posToLineA, lineAToB); // lineBToA stores projected vec.

		float mplier = glm::dot(lineAToB, lineAToB);
		SR_Assert(mplier > 0.000000001f);	// Zero divisor check.
		mplier = glm::dot(posToLineA, lineAToB) / mplier;

		lineAToB = lineAToB * mplier;

		// Clamp this point to between A and B.
		if (mplier <= 0.0f)
		{
			// Before A.
			tmpClosest = lineA;
		}
		else if (mplier >= 1.0f)
		{
			// After B.
			tmpClosest = lineB;
		}
		else
		{
			// Between A and B.
			tmpClosest = lineA + lineAToB;
		}

		// Calculate distance to that point.

		glm::vec3 diff = tmpClosest - source;
		tmpDistance = glm::dot(diff, diff);
		if (tmpDistance < closestDistSqr)
		{
			secondClosestDistSqr = closestDistSqr;
			secondClosestPoint = closestPoint;
			secondClosestIndex = closestIndex;

			closestDistSqr = tmpDistance;
			closestPoint = tmpClosest;
			closestIndex = i;
		}
		else if (tmpDistance < secondClosestDistSqr && tmpDistance != closestDistSqr)
		{
			secondClosestDistSqr = tmpDistance;
			secondClosestPoint = tmpClosest;
			secondClosestIndex = i;
		}
	}

	outClosestPoint = closestPoint;
	if (outSecondClosestPoint != nullptr)
	{
		*outSecondClosestPoint = secondClosestPoint;
	}
	if (outClosestLineVertex != nullptr)
	{
		// Iterate thru all points to 
		*outClosestLineVertex = lines[glm::max<size_t>(closestIndex, secondClosestIndex)];
	}
}

/*
const meVector3 eyePos = GetListenerPosition();

meVector3 closestPoint = meVector3(FLT_MAX, FLT_MAX, FLT_MAX);
float closestDistSqr = FLT_MAX;

meVector3 secondClosestPoint = meVector3(FLT_MAX, FLT_MAX, FLT_MAX);
float secondClosestDistSqr = FLT_MAX;

sguint32 closestIndex = 0;
float closestDistanceToPoint = FLT_MAX;

float tmpDistance;
meVector3 tmpClosest;
// Line strip.
for (sguint32 i = 0; i < (soundPositionsNum - 1); i++)
{
assert(i < soundPositionsNum && (i + 1) < soundPositionsNum);

meVector3 lineA = soundPositions[i];
meVector3 lineB = soundPositions[i + 1];
ConvertHandedness(lineA);
ConvertHandedness(lineB);

// Calculate distanceSqr to these points to obtain closestIndex
{
const float distToA = meDot(lineA - eyePos, lineA - eyePos);
const float distToB = meDot(lineB - eyePos, lineB - eyePos);

if (distToA < closestDistanceToPoint)
{
closestDistanceToPoint = distToA;
closestIndex = i;
}
if (distToB < closestDistanceToPoint)
{
closestDistanceToPoint = distToB;
closestIndex = i + 1;
}
}

// Project point onto a line.
meVector3 posToLineA = eyePos - lineA;
meVector3 lineAToB = lineB - lineA;


//float mplier = sgvector_project3(lineAToB, posToLineA, lineAToB); // lineBToA stores projected vec.

float mplier = meDot(lineAToB, lineAToB);
assert(mplier > 0.000000001f);	// Zero divisor check.
mplier = meDot(posToLineA, lineAToB) / mplier;

lineAToB = lineAToB * mplier;

// Clamp this point to between A and B.
if (mplier <= 0.0f)
{
// Before A.
tmpClosest = lineA;
}
else if (mplier >= 1.0f)
{
// After B.
tmpClosest = lineB;
}
else
{
// Between A and B.
tmpClosest = lineA + lineAToB;
}

// Calculate distance to that point.

meVector3 diff = tmpClosest - eyePos;
tmpDistance = meDot(diff, diff);
if (tmpDistance < closestDistSqr)
{
secondClosestDistSqr = closestDistSqr;
secondClosestPoint = closestPoint;

closestDistSqr = tmpDistance;
closestPoint = tmpClosest;
}
else if (tmpDistance < secondClosestDistSqr && tmpDistance != closestDistSqr)
{
secondClosestDistSqr = tmpDistance;
secondClosestPoint = tmpClosest;
}
}


// Compute a border point when we are close to switching points
meVector3 nearPointPosition = closestPoint;
if(soundPositionsNum > 2 && closestIndex > 0 && closestIndex < (soundPositionsNum - 1))
{
const float DIFF_THRESHOLD = 5.0f;
const float diffSqr = fabsf(closestDistSqr - secondClosestDistSqr);
if (diffSqr < DIFF_THRESHOLD)
{
const meVector3 middlePoint = (closestPoint + secondClosestPoint) * 0.5f;
const float lerpVal = 1.0f - diffSqr / DIFF_THRESHOLD;
nearPointPosition = meLerp(lerpVal, closestPoint, middlePoint);
}
}

return nearPointPosition;
*/