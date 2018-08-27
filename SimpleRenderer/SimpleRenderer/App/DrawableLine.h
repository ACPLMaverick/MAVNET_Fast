#pragma once

#include "Drawable.h"
#include "DrawableMesh.h"

class DrawableLine : public Drawable
{
public:

	class Params : public Drawable::Params
	{
	public:
		std::vector<glm::vec3> Points;
		glm::vec4 Color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		float Width = 1.0f;
	};

	DrawableLine();
	virtual ~DrawableLine();

	virtual void Init(Drawable::Params* params) override;
	virtual void Shutdown() override;
	virtual void Update() override;
	virtual void Draw() const override;

protected:

	DrawableMesh _meshPoints;
	DrawableMesh _meshLines;
};

