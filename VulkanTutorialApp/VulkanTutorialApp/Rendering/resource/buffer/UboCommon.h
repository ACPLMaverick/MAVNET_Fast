#pragma once

namespace Rendering
{
	namespace UboCommon
	{
		struct TransformData
		{
			glm::mat4 WVP;
			glm::mat4 WV;
			glm::mat4 WVInverseTranspose;

			TransformData()
				: WVP(glm::mat4(1.0f))
				, WV(glm::mat4(1.0f))
				, WVInverseTranspose(glm::mat4(1.0f))
			{
			}
		};

		struct SceneLighting
		{
			JE_AlignAs(16) glm::vec3 LightColor;
			JE_AlignAs(16) glm::vec3 InvLightDirectionV;
			float FogDistNear;
			glm::vec3 FogColor;
			float FogDistFar;
		};
	}
}