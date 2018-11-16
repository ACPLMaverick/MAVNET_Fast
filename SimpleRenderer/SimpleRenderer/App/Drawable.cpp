#include "Drawable.h"


const uint16_t Drawable::DrawModeToGLArray[(size_t)DrawMode::ENUM_SIZE]
{
	GL_POINTS,
	GL_LINES,
	GL_TRIANGLES
};

Drawable::Drawable()
	: _params(nullptr)
	, _bParamsChanged(false)
{
}


Drawable::~Drawable()
{
}

void Drawable::Init(Params* params)
{
	SR_Assert(_params == nullptr);
	SR_Assert(params != nullptr);
	_params = params;
}

void Drawable::Shutdown()
{
	SR_CleanSimpleObject(_params);
}
