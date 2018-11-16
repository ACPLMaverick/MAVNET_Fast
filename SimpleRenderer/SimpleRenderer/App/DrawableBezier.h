#pragma once

#include "DrawableSingleObject.h"

class DrawableBezier : public DrawableSingleObject
{
public:

	class Params : public DrawableSingleObject::Params
	{
	public:
		std::vector<glm::vec3> Points;
		float Width = 1.0f;
	};

	struct Bezier
	{
		glm::vec3 P0 = glm::vec3(0.0f);
		glm::vec3 P1 = glm::vec3(0.0f);
		glm::vec3 P2 = glm::vec3(0.0f);
		glm::vec3 P3 = glm::vec3(0.0f);
	};

	struct BlendParam
	{
		float B0;
		float B1;
		float B2;
		float B3;

		BlendParam()
			: B0(1.0f)
			, B1(0.0f)
			, B2(0.0f)
			, B3(0.0f)
		{
		}

		BlendParam(float t)
		{
			if (t < 0.0f)
				t = 0.0f;
			else if (t > 1.0f)
				t = 1.0f;

			B0 = (1.0f - t) * (1.0f - t) * (1.0f - t);
			B1 = 3.0f * t * (1.0f - t) * (1.0f - t);
			B2 = 3.0f * t * t * (1.0f - t);
			B3 = t * t * t;
		}
	};

	DrawableBezier();
	virtual ~DrawableBezier();

	virtual void Init(Drawable::Params* params) override;
	virtual void Draw() const override;


	const std::vector<BlendParam>& GetBlendParams() const { return _blendParams; }
	const std::vector<Bezier>& GetBeziers() const { return _beziers; }
	const std::vector<glm::vec3>& GetInterpolatedPoints() const { return _interpolatedPoints; }

	static glm::vec3 InterpolateBezier(const Bezier& bezier, const BlendParam& blendParam);

protected:

	virtual void CreateVertices() override;

	std::vector<BlendParam> _blendParams;

	std::vector<Bezier> _beziers;
	std::vector<glm::vec3> _interpolatedPoints;
};

