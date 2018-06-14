#include "SystemDrawable.h"


namespace Rendering
{
	JE_DefineClientClassBody(Drawable, Util::NullType);

	Drawable::Drawable() 
		: _mesh(nullptr)
		, _material(nullptr)
	{

	}

	Drawable::Drawable(const Drawable& other)
		: _mesh(other._mesh)
		, _material(other._material)
	{

	}

	Drawable::Drawable(const Drawable&& otherMove)
		: _mesh(otherMove._mesh)
		, _material(otherMove._material)
	{

	}

	Drawable::~Drawable()
	{
		JE_Assert(_mesh == nullptr);
		JE_Assert(_material == nullptr);
	}

	Drawable& Drawable::operator=(const Drawable& other)
	{
		_mesh = other._mesh;
		_material = other._material;
		return *this;
	}

	Drawable& Drawable::operator=(const Drawable&& otherMove)
	{
		_mesh = otherMove._mesh;
		_material = otherMove._material;
		return *this;
	}

	JE_DefineSystemClassBody(Drawable);
}