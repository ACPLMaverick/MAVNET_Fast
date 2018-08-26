#pragma once

#include "Drawable.h"

class DrawableSingleObject : public Drawable
{
public:

	struct Vertex
	{
		glm::vec3 Position = glm::vec3(0.0f);
	};

	class Params : public Drawable::Params
	{
	public:
		glm::vec4 Color = glm::vec4(1.0f);
		glm::vec3 Position = glm::vec3(0.0f);
		float Rotation = 0.0f;
		float Scale = 1.0f;
		DrawMode DrwMode = DrawMode::Triangles;

		Params() : Drawable::Params() {}
		virtual ~Params() {}
	};

	DrawableSingleObject();
	virtual ~DrawableSingleObject();

	virtual void Init(Drawable::Params* params) override;
	virtual void Shutdown() override;
	virtual void Update() override;
	virtual void Draw() const override;

protected:

	virtual void UpdateParams();
	virtual glm::mat4 BuildScale() const;
	virtual glm::mat4 BuildRotation() const;
	virtual glm::mat4 BuildTranslation() const;

	virtual void CreateVertices() = 0;


	std::vector<Vertex> _verticesCpu;

	glm::mat4 _matM;

	GLuint _vertices; // Vertex Buffer Object
	GLuint _vao; // Vertex Array Object
};

