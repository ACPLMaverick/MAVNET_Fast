#pragma once

#include "GlobalIncludes.h"

namespace Core
{
	class Fog
	{
	public:

		Fog();
		~Fog();

		void Initialize(const glm::vec3* color, float startDistance, float endDistance);
		void Shutdown();

		void Update();

		void SetColor(const glm::vec3* color) { _color = *color; }
		
		void SetStartDistance(float startDistance);
		void SetEndDistance(float endDistance);

		const glm::vec3* GetColor() const { return &_color; }

		float GetStartDistance() const { return _startDistance; }
		float GetEndDistance() const { return _endDistance; }

		float GetStartDepth() const { return _startDepth; }
		float GetEndDepth() const { return _endDepth; }

	private:

		void UpdateDepthDistances();

		glm::vec3 _color;

		float _startDistance;
		float _endDistance;

		float _startDepth;
		float _endDepth;

		bool _bNeedUpdateDepthDistances;
	};
}
