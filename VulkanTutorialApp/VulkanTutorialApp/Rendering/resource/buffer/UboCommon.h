#pragma once

namespace Rendering
{
	namespace UboCommon
	{
		struct StaticMeshCommon
		{
			glm::mat4 MVP;
			glm::mat4 MV;
			glm::mat4 MVInverseTranspose;

			StaticMeshCommon()
				: MVP(glm::mat4(1.0f))
				, MV(glm::mat4(1.0f))
				, MVInverseTranspose(glm::mat4(1.0f))
			{
			}
		};

		struct SceneGlobal
		{
			JE_AlignAs(16) glm::vec3 LightColor;
			JE_AlignAs(16) glm::vec3 InvLightDirectionV;
			float FogDistNear;
			glm::vec3 FogColor;
			float FogDistFar;
		};
	}
}