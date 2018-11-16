#pragma once

#include "DrawableSingleObject.h"

class DrawablePoint : public DrawableSingleObject
{
public:

	class Params : public DrawableSingleObject::Params
	{
	};

	DrawablePoint();
	virtual ~DrawablePoint();

	virtual void Draw() const override;

protected:

	virtual void CreateVertices() override;
};

