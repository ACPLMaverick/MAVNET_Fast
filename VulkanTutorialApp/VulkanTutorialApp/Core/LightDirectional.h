#pragma once

#include "GlobalIncludes.h"

namespace Core
{
	class LightDirectional
	{
	public:

		LightDirectional();
		virtual ~LightDirectional();

		void Initialize(const glm::vec3* color, const glm::vec3* direction);
		void Shutdown();

		void Update();

		void SetColor(const glm::vec3* color) { _color = *color; }
		void SetDirection(const glm::vec3* direction);

		const glm::vec3* GetColor() const { return &_color; }
		const glm::vec3* GetDirection() const { return &_direction; }
		const glm::vec3* GetDirectionV() const { return &_directionV; }

	protected:

		glm::vec3 _color;
		glm::vec3 _direction;
		glm::vec3 _directionV;

		bool _bNeedUpdateDirectionV;

	};
}

