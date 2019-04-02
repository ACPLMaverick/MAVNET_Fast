#pragma once

#include "Rendering/resource/buffer/UboCommon.h"
#include "Rendering/LightDirectional.h"
#include "Rendering/Fog.h"

namespace Rendering
{
	class UniformBuffer;
}

namespace GOM
{
	class GlobalWorldParameters
	{
	public:

		GlobalWorldParameters()
			: _uboLighting(nullptr)
			, _clearColor(0.0f, 0.0f, 0.0f, 1.0f)
		{
		}

		~GlobalWorldParameters()
		{
		}

		void Initialize();
		void Cleanup();
		void Update();

		glm::vec4* GetClearColorForEdit() { _bNeedReapply = true; return &_clearColor; }
		::Rendering::LightDirectional* GetSunLightForEdit() { _bNeedUpdateSunlight = true; _bNeedReapply = true; return &_sunLight; }
		::Rendering::Fog* GetFogForEdit() { _bNeedUpdateFog = true; _bNeedReapply = true; return &_fog; }

		const glm::vec4* GetClearColor() const { return &_clearColor; }
		const ::Rendering::LightDirectional* GetSunLight() const { return &_sunLight; }
		const ::Rendering::Fog* GetFog() const { return &_fog; }

		const ::Rendering::UniformBuffer* GetUboLighting() const { return _uboLighting; }

	private:

		void ApplyLighting();


		glm::vec4 _clearColor;
		::Rendering::LightDirectional _sunLight;
		::Rendering::Fog _fog;

		::Rendering::UboCommon::SceneLighting _dataLighting;
		::Rendering::UniformBuffer* _uboLighting;

		bool _bNeedReapply = false;
		bool _bNeedUpdateFog = false;
		bool _bNeedUpdateSunlight = false;
	};
}