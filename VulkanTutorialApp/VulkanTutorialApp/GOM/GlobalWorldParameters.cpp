#include "GlobalWorldParameters.h"

#include "Rendering/resource/buffer/UniformBuffer.h"

namespace GOM
{

	void GlobalWorldParameters::Initialize()
	{
		JE_Assert(!_uboLighting);

		Rendering::UniformBuffer::Options options;
		options.DataSize = sizeof(_dataLighting);
		_uboLighting = new Rendering::UniformBuffer();
		_uboLighting->Initialize(&options);
	}

	void GlobalWorldParameters::Cleanup()
	{
		JE_Assert(_uboLighting);
		_uboLighting->Cleanup();
		delete _uboLighting;

		_fog.Shutdown();
		_sunLight.Shutdown();
	}

	void GlobalWorldParameters::Update()
	{
		if (_bNeedUpdateSunlight)
		{
			_sunLight.Update();
		}

		if (_bNeedUpdateFog)
		{
			_fog.Update();
		}

		if (_bNeedReapply)
		{
			ApplyLighting();
			_bNeedReapply = false;
		}
	}

	void GlobalWorldParameters::ApplyLighting()
	{
		_dataLighting.FogColor = *_fog.GetColor();
		_dataLighting.FogDistNear = _fog.GetStartDistance();
		_dataLighting.FogDistFar = _fog.GetEndDistance();
		_dataLighting.LightColor = *_sunLight.GetColor();
		_dataLighting.InvLightDirectionV = -*_sunLight.GetDirectionV();

		_uboLighting->UpdateWithData(reinterpret_cast<uint8_t*>(&_dataLighting), sizeof(_dataLighting));
	}

}