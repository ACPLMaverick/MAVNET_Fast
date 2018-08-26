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
	lineParams->Width = 3.5f;
	lineParams->Points.push_back(glm::vec3(-8.0f, 2.0f, 0.0f));
	lineParams->Points.push_back(glm::vec3(-4.0f, -2.0f, 0.0f));
	lineParams->Points.push_back(glm::vec3(0.0f, 6.0f, 0.0f));
	lineParams->Points.push_back(glm::vec3(5.0f, 3.0f, 0.0f));
	lineParams->Points.push_back(glm::vec3(6.0f, 0.0f, 0.0f));

	line->Init(lineParams);
	_line = line;


	point = new DrawablePoint();
	_drawables.push_back(point);

	DrawablePoint::Params* paramsMarker = new DrawablePoint::Params();
	paramsMarker->Shd = shdColor;
	paramsMarker->DrwMode = Drawable::DrawMode::Points;
	paramsMarker->Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	paramsMarker->Scale = 10.0f;

	point->Init(paramsMarker);
	_markerLine = point;

	point = new DrawablePoint();
	_drawables.push_back(point);

	DrawablePoint::Params* paramsMarkerBezier = new DrawablePoint::Params();
	paramsMarkerBezier->Shd = shdColor;
	paramsMarkerBezier->DrwMode = Drawable::DrawMode::Points;
	paramsMarkerBezier->Color = glm::vec4(1.0f, 0.8f, 0.1f, 1.0f);
	paramsMarkerBezier->Scale = 12.5f;
	paramsMarkerBezier->Position = glm::vec3(0.0f, -8.0f, 0.0f);

	point->Init(paramsMarkerBezier);
	_markerBezier = point;
}

void SceneBezier::UpdateLogic()
{
	MovePlayer();
	ComputeLineMarker();
	ComputeBezierMarker();
}

void SceneBezier::ComputeBezierPoints(const std::vector<glm::vec3>& linePoints, std::vector<glm::vec3>& outBezier)
{
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
}

void SceneBezier::ComputeBezierMarker()
{
}
