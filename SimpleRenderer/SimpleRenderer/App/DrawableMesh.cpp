#include "DrawableMesh.h"



DrawableMesh::DrawableMesh() : DrawableSingleObject()
{
}


DrawableMesh::~DrawableMesh()
{
}

void DrawableMesh::CreateVertices()
{
	Params* params = (Params*)_params;

	if (params->LoadName.size() > 0)
	{
		Load(params->LoadName);
	}
	else if (params->LoadVertices.size() > 0)
	{
		Load(params->LoadVertices);
	}
	else
	{
		SR_Assert(false);
	}
}

void DrawableMesh::Load(const std::string & name)
{
	SR_Assert(false); // TODO: Implement.
}

void DrawableMesh::Load(const std::vector<DrawableSingleObject::Vertex>& vertices)
{
	for (const Vertex& vtx : vertices)
	{
		_verticesCpu.push_back(vtx);
	}
}
