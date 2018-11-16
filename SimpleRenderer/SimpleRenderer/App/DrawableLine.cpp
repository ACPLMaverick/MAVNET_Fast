#include "DrawableLine.h"



DrawableLine::DrawableLine() : Drawable()
{
}


DrawableLine::~DrawableLine()
{
}

void DrawableLine::Init(Drawable::Params * params)
{
	Drawable::Init(params);

	Params* thisParams = (Params*)params;

	DrawableMesh::Params* pointParams = new DrawableMesh::Params();
	pointParams->Shd = thisParams->Shd;
	pointParams->Color = thisParams->Color;
	pointParams->DrwMode = DrawMode::Points;

	for (const glm::vec3& vtx : thisParams->Points)
	{
		pointParams->LoadVertices.push_back({ vtx });
	}

	_meshPoints.Init(pointParams);


	DrawableMesh::Params* lineParams = new DrawableMesh::Params();
	lineParams->Shd = thisParams->Shd;
	lineParams->Color = thisParams->Color;
	lineParams->DrwMode = DrawMode::Lines;

	SR_Assert(thisParams->Points.size() > 1);

	for (size_t i = 0; i < thisParams->Points.size() - 1; ++i)
	{
		lineParams->LoadVertices.push_back({ thisParams->Points[i] });
		lineParams->LoadVertices.push_back({ thisParams->Points[i + 1] });
	}

	_meshLines.Init(lineParams);
}

void DrawableLine::Shutdown()
{
	_meshPoints.Shutdown();
	_meshLines.Shutdown();

	Drawable::Shutdown();
}

void DrawableLine::Update()
{
	if (_bParamsChanged)
	{
		Params* thisParams = (Params*)_params;
		((DrawableMesh::Params&)_meshPoints.GetParams()).Color = thisParams->Color;
		((DrawableMesh::Params&)_meshPoints.GetParams()).Scale = thisParams->Width * 2.0f;
		((DrawableMesh::Params&)_meshLines.GetParams()).Color = thisParams->Color;
		((DrawableMesh::Params&)_meshLines.GetParams()).Scale = thisParams->Width;
		_bParamsChanged = false;
	}
	_meshPoints.Update();
	_meshLines.Update();
}

void DrawableLine::Draw() const
{
	glPointSize(((Params*)_params)->Width * 2.0f);
	_meshPoints.Draw();
	glLineWidth(((Params*)_params)->Width);
	_meshLines.Draw();
}
