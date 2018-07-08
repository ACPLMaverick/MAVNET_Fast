#pragma once

#include "Rendering/resource/Mesh.h"
#include "Rendering/resource/Texture.h"
#include "Rendering/resource/Material.h"

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