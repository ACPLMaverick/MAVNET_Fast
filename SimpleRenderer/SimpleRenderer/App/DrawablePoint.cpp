#include "DrawablePoint.h"



DrawablePoint::DrawablePoint()
	: DrawableSingleObject()
{
}


DrawablePoint::~DrawablePoint()
{
}

void DrawablePoint::Draw() const
{
	glPointSize(((Params*)_params)->Scale);

	DrawableSingleObject::Draw();
}

void DrawablePoint::CreateVertices()
{
	_verticesCpu.push_back({ glm::vec3(0.0f) });
	/*
	static const float BASE = 4.0f;

	if (_params->DrwMode == DrawMode::Lines)
	{
		_verticesCpu.push_back({ glm::vec3(-BASE, -BASE, 0.0f) });
		_verticesCpu.push_back({ glm::vec3(BASE, -BASE, 0.0f) });
		_verticesCpu.push_back({ glm::vec3(BASE, -BASE, 0.0f) });
		_verticesCpu.push_back({ glm::vec3(0.0f, BASE, 0.0f) });
		_verticesCpu.push_back({ glm::vec3(0.0f, BASE, 0.0f) });
		_verticesCpu.push_back({ glm::vec3(-BASE, -BASE, 0.0f) });
	}
	else
	{
		_verticesCpu.push_back({ glm::vec3(-BASE, -BASE, 0.0f) });
		_verticesCpu.push_back({ glm::vec3(BASE, -BASE, 0.0f) });
		_verticesCpu.push_back({ glm::vec3(0.0f, BASE, 0.0f) });
	}
	*/
}
