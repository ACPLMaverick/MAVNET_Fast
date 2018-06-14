#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

namespace Rendering
{
	JE_DeclareClientClass(Drawable, Util::NullType)
	{
		JE_DeclareClientClassBody(Drawable, Util::NullType);

		Mesh* _mesh;
		Material* _material;
	};

	JE_DeclareSystemClass(Drawable)
	{
		JE_DeclareSystemClassBody(Drawable);
	};
}