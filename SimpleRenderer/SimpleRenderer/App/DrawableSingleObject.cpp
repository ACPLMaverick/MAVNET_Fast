#include "DrawableSingleObject.h"

#include "Shader.h"
#include "App.h"
#include "Camera.h"

DrawableSingleObject::DrawableSingleObject()
	: _matM(glm::mat4(1.0f))
	, _vertices(0)
	, _vao(0)
{
}


DrawableSingleObject::~DrawableSingleObject()
{
}

void DrawableSingleObject::Init(Drawable::Params* params)
{
	Drawable::Init(params);

	CreateVertices();
	SR_Assert(_verticesCpu.size() > 0);

	glGenBuffers(1, &_vertices);

	glGenVertexArrays(1, &_vao);

	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _verticesCpu.size(), _verticesCpu.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	_bParamsChanged = true;
}

void DrawableSingleObject::Shutdown()
{
	Drawable::Shutdown();
}

void DrawableSingleObject::Update()
{
	UpdateParams();
}

void DrawableSingleObject::Draw() const
{
	_params->Shd->Use();

	glm::mat4 mvp = App::GetCam().GetVP() * _matM;

	_params->Shd->SetUniformMVP(mvp);
	_params->Shd->SetUniformColor(((Params*)_params)->Color);

	glBindVertexArray(_vao);
	glDrawArrays(DrawModeToGL(((Params*)_params)->DrwMode), 0, (GLsizei)_verticesCpu.size());
}

void DrawableSingleObject::UpdateParams()
{
	if (!_bParamsChanged)
		return;

	// Rebuild m matrix
	glm::mat4& mMatM = const_cast<glm::mat4&>(_matM);

	mMatM = BuildTranslation() * BuildRotation() * BuildScale();

	const_cast<DrawableSingleObject*>(this)->_bParamsChanged = false;
}

glm::mat4 DrawableSingleObject::BuildScale() const
{
	Params* params = reinterpret_cast<Params*>(_params);
	const glm::mat4 one(1.0f);
	return glm::scale(one, glm::vec3(params->Scale, params->Scale, params->Scale));
}

glm::mat4 DrawableSingleObject::BuildRotation() const
{
	Params* params = reinterpret_cast<Params*>(_params);
	const glm::mat4 one(1.0f);
	return glm::rotate(one, glm::radians(params->Rotation), glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::mat4 DrawableSingleObject::BuildTranslation() const
{
	Params* params = reinterpret_cast<Params*>(_params);
	const glm::mat4 one(1.0f);
	return glm::translate(one, params->Position);
}
