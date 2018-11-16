#pragma once

#include "DrawableSingleObject.h"

class DrawableMesh : public DrawableSingleObject
{
public:

	class Params : public DrawableSingleObject::Params
	{
	public:
		std::string LoadName = "";	// If this is not void, model will be loaded from file regardless of the Vertices passed.
		std::vector<Vertex> LoadVertices;
	};

	DrawableMesh();
	virtual ~DrawableMesh();

protected:

	virtual void CreateVertices() override;

	virtual void Load(const std::string& name);
	virtual void Load(const std::vector<DrawableSingleObject::Vertex>& vertices);
};

