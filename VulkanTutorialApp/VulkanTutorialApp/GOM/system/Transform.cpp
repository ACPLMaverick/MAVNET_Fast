#include "Transform.h"

#include "Core/HelloTriangle.h"
#include "Rendering/resource/buffer/UniformBuffer.h"

namespace GOM
{
	void Transform::OnMovabilityAboutToChange(const Movability& oldValue, const Movability& newValue)
	{
		JE_GetSystem()->GetBehaviour<TransformBehaviour>()->OnTransformMovabilityAboutToChange(this, newValue);
	}

	Transform::Data& Transform::GetData()
	{
		return JE_GetApp()->GetSystem()->GetBehaviour<TransformBehaviour>()->GetDataOfTransform(this);
	}

	void TransformBehaviour::Update()
	{
		// Regenerate matrices for dynamic objects.
		Util::ObjectPool<Transform::Data>& dynamicData = _transformDataPerMovability[(size_t)Transform::Movability::Dynamic];
		const size_t dynamicDataMaxIndex = dynamicData.GetMaxIndex();
		Transform::Data* dynamicDataArray = dynamicData.GetArray();

		for (size_t i = 0; i <= dynamicDataMaxIndex; ++i)
		{
			ProcessTransformData(dynamicDataArray[i]);
		}

		// TODO have UBOS in array like transform data... Or even have UBO as a part of transform data.
		if (_bNeedUpdateUbos)
		{
			for (Component* component : _componentsAll)
			{
				UpdateUboTransform(ComponentCast(component));
			}

			_bNeedUpdateUbos = false;
		}
		else
		{
			for (Component* component : _componentsAll)
			{
				Transform* transform = ComponentCast(component);
				if (transform->PropMovability.Get() == Transform::Movability::Dynamic)
				{
					UpdateUboTransform(transform);
				}
			}
		}
	}

	void TransformBehaviour::Draw()
	{
		// Does not get drawn.
	}

	Component * TransformBehaviour::ConstructComponent_Internal(const ComponentConstructionParameters* constructionParam)
	{
		TransfromConstructionParameters parameters;

		if (constructionParam)
		{
			parameters = *static_cast<const TransfromConstructionParameters*>(constructionParam);
		}

		Transform* transform = new Transform(parameters.InitMovability);
		JE_SetPropertyPtr(transform, _indexData, _transformDataPerMovability[(size_t)transform->PropMovability.Get()].Create());

		return transform;
	}

	void TransformBehaviour::InitializeComponent_Internal(Component * objAbstract)
	{
		Transform* transform = ComponentCast(objAbstract);

		// Perform a first update of a world matrix. If this is a static object, this will also be the last update.
		ProcessTransformData(transform->GetData());

		CreateUboTransform(transform);
		UpdateUboTransform(transform);
	}

	void TransformBehaviour::BindComponentWithOwner(Component* objAbstract, Entity* owner)
	{
		Transform* transform = ComponentCast(objAbstract);
		transform->_owner = owner;
		owner->AddComponent(transform);
	}

	void TransformBehaviour::CleanupComponent_Internal(Component * objAbstract)
	{
		Transform* transform = ComponentCast(objAbstract);
		CleanupUboTransform(transform);
		_transformDataPerMovability[(size_t)transform->PropMovability.Get()].Free(transform->_indexData.Get());
	}

	void TransformBehaviour::CloneComponent_Internal(Component * destinationAbstract, const Component * sourceAbstract)
	{
		Transform* transformDestination = ComponentCast(destinationAbstract);
		const Transform* transformSource = ComponentCast(sourceAbstract);

		Util::ObjectPool<Transform::Data>& srcDataArray = _transformDataPerMovability[(size_t)transformSource->PropMovability.Get()];

		JE_SetPropertyPtr(transformDestination, _indexData, srcDataArray.Copy(srcDataArray, transformSource->_indexData.Get()));

		// Perform a first update of a world matrix. If this is a static object, this will also be the last update.
		ProcessTransformData(transformDestination->GetData());

		// Must create ubo anew.
		CreateUboTransform(transformDestination);
		UpdateUboTransform(transformDestination);
	}

	void TransformBehaviour::OnSwapChainResize_Internal(Component* obj)
	{
		_bNeedUpdateUbos = true;
	}

	void TransformBehaviour::OnTransformMovabilityAboutToChange(Transform* transform, Transform::Movability newValue)
	{
		CheckComponent(transform);

		Transform::Movability oldValue = transform->PropMovability.Get();
		JE_SetPropertyPtr(transform, _indexData, _transformDataPerMovability[(size_t)newValue]
			.Move(_transformDataPerMovability[(size_t)oldValue], transform->_indexData.Get()));
	}

	GOM::Transform::Data& TransformBehaviour::GetDataOfTransform(Transform* transform)
	{
		CheckComponent(transform);
		return _transformDataPerMovability[(size_t)transform->PropMovability.Get()].Get(transform->_indexData.Get());
	}

	JE_Inline void TransformBehaviour::ProcessTransformData(Transform::Data& data)
	{
		data.WLocal = glm::scale(glm::mat4(1.0f), data.Scale);
		data.WLocal = glm::rotate(data.WLocal, data.Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		data.WLocal = glm::rotate(data.WLocal, data.Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		data.WLocal = glm::rotate(data.WLocal, data.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		data.WLocal = glm::translate(data.WLocal, data.Position);

		// TODO: Hierarchy.
		data.W = data.WLocal;

		// TODO: THIS HAS TO BE UPDATED FOR BOTH DYNAMIC AND STATIC OBJECTS!!!
		const glm::mat4& viewProj = *JE_GetRenderer()->GetCamera()->GetViewProj();
		const glm::mat4& view = *JE_GetRenderer()->GetCamera()->GetView();

		data.WVP = viewProj * data.W;
		data.WV = view * data.W;
		data.WVInverseTranspose = glm::transpose(glm::inverse(data.WV));
	}

	JE_Inline void TransformBehaviour::CreateUboTransform(Transform* transform)
	{
		Rendering::UniformBuffer::Options options;
		options.DataSize = sizeof(Rendering::UboCommon::TransformData);
		transform->_uboTransform = new Rendering::UniformBuffer();
		transform->_uboTransform->Initialize(&options);
	}

	JE_Inline void TransformBehaviour::CleanupUboTransform(Transform* transform)
	{
		transform->_uboTransform->Cleanup();
		delete transform->_uboTransform;
		transform->_uboTransform = nullptr;
	}

	JE_Inline void TransformBehaviour::UpdateUboTransform(Transform* transform)
	{
		Transform::Data& data = GetDataOfTransform(transform);
		transform->_uboTransform->UpdateWithData(reinterpret_cast<uint8_t*>(&data), sizeof(Rendering::UboCommon::TransformData));
	}

#if JE_BEHAVIOUR_CHECK_COMPONENT
	void TransformBehaviour::CheckComponent(const GOM::Component *objAbstract)
	{
		const Transform* transform = ComponentCast(objAbstract);
		JE_Assert(transform->_indexData.Get() != Util::ObjectPool<Transform::Data>::BAD_INDEX);
	}
#endif
}