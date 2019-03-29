#include "Transform.h"

#include "Core/HelloTriangle.h"

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
		transform->_indexData = _transformDataPerMovability[(size_t)transform->PropMovability.Get()].Create();

		return transform;
	}

	void TransformBehaviour::InitializeComponent_Internal(Component * objAbstract)
	{
		Transform* transform = ComponentCast(objAbstract);

		// Perform a first update of a world matrix. If this is a static object, this will also be the last update.
		ProcessTransformData(transform->GetData());
	}

	void TransformBehaviour::CleanupComponent_Internal(Component * objAbstract)
	{
		Transform* transform = ComponentCast(objAbstract);
		_transformDataPerMovability[(size_t)transform->PropMovability.Get()].Free(transform->_indexData);
	}

	void TransformBehaviour::CloneComponent_Internal(Component * destinationAbstract, const Component * sourceAbstract)
	{
		Transform* transformDestination = ComponentCast(destinationAbstract);
		const Transform* transformSource = ComponentCast(sourceAbstract);

		Util::ObjectPool<Transform::Data>& srcDataArray = _transformDataPerMovability[(size_t)transformSource->PropMovability.Get()];

		transformDestination->_indexData = srcDataArray.Copy(srcDataArray, transformSource->_indexData);
	}

	void TransformBehaviour::OnTransformMovabilityAboutToChange(Transform* transform, Transform::Movability newValue)
	{
		CheckComponent(transform);

		Transform::Movability oldValue = transform->PropMovability.Get();
		transform->_indexData = _transformDataPerMovability[(size_t)newValue]
			.Move(_transformDataPerMovability[(size_t)oldValue], transform->_indexData);
	}

	GOM::Transform::Data& TransformBehaviour::GetDataOfTransform(Transform* transform)
	{
		CheckComponent(transform);
		return _transformDataPerMovability[(size_t)transform->PropMovability.Get()].Get(transform->_indexData);
	}

	JE_Inline void TransformBehaviour::ProcessTransformData(Transform::Data& data)
	{
		data.WorldMatrix = glm::scale(glm::mat4::mat(), data.Scale);
		data.WorldMatrix = glm::rotate(data.WorldMatrix, data.Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		data.WorldMatrix = glm::rotate(data.WorldMatrix, data.Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		data.WorldMatrix = glm::rotate(data.WorldMatrix, data.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		data.WorldMatrix = glm::translate(data.WorldMatrix, data.Position);
	}

#if JE_BEHAVIOUR_CHECK_COMPONENT
	void TransformBehaviour::CheckComponent(const GOM::Component *objAbstract)
	{
		const Transform* transform = ComponentCast(objAbstract);
		JE_Assert(transform->_indexData != Util::ObjectPool<Transform::Data>::BAD_INDEX);
	}
#endif
}