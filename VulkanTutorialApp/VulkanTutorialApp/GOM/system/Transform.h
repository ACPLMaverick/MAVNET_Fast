#pragma once

#include "System.h"
#include "Rendering/resource/buffer/UboCommon.h"

namespace Rendering
{
	class UniformBuffer;
}

namespace GOM
{
	class TransformBehaviour;

	class Transform : public Component
	{
		JE_System_Component_Body_Declaration(TransformBehaviour, Transform)

	public:

		JE_EnumBegin(Movability)
			Static,
			Dynamic
		JE_EnumEnd()

		struct Data : public ::Rendering::UboCommon::TransformData
		{
			glm::mat4 W = glm::mat4(1.0f);
			glm::mat4 WLocal = glm::mat4(1.0f);

			glm::vec3 Position = glm::vec3(0.0f);
			glm::vec3 Rotation = glm::vec3(0.0f);
			glm::vec3 Scale = glm::vec3(1.0f);

			Rendering::UniformBuffer* UboTransform = nullptr;
		};

	protected:

		void OnMovabilityAboutToChange(const Movability& oldValue, const Movability& newValue);

	public:

		Util::Property<Movability, Transform, nullptr, &Transform::OnMovabilityAboutToChange> PropMovability;

		const Data& GetData() const { return const_cast<Transform*>(this)->GetData_Internal(); }
		glm::vec3& GetPosition() { return GetData_Internal().Position; }
		glm::vec3& GetRotation() { return GetData_Internal().Rotation; }
		glm::vec3& GetScale() { return GetData_Internal().Scale; }

		void SetPosition(const glm::vec3& position) { GetData_Internal().Position = position; }
		void SetRotation(const glm::vec3& rotation) { GetData_Internal().Rotation = rotation; }
		void SetScale(const glm::vec3& scale) { GetData_Internal().Scale = scale; }

		void AddPosition(const glm::vec3& positionOffset) { GetData_Internal().Position += positionOffset; }
		void AddRotation(const glm::vec3& rotationOffset) { GetData_Internal().Rotation += rotationOffset; }
		void AddScale(const glm::vec3& scaleOffset) { GetData_Internal().Scale += scaleOffset; }

	protected:

		Transform(Movability movability)
			: Component()
			, PropMovability(movability)
			, _indexData(Util::ObjectPool<Data>::BAD_INDEX)
		{
		}
		virtual ~Transform()
		{
		}

		Data& GetData_Internal();

		Util::Property<Util::ObjectPool<Data>::Index, Transform> _indexData;
	};

	class TransfromConstructionParameters : public ComponentConstructionParameters
	{
	public:
		Transform::Movability InitMovability = Transform::Movability::Static;
	};

	class TransformBehaviour : public Behaviour
	{
		JE_System_Behaviour_Body_Declaration(TransformBehaviour, Transform)

	public:

		// Inherited via Behaviour
		virtual void Update() override;
		virtual void Draw() override;

	protected:

		virtual Component * ConstructComponent_Internal(const ComponentConstructionParameters* constructionParam) override;
		virtual void InitializeComponent_Internal(Component * obj) override;
		virtual void BindComponentWithOwner(Component* obj, Entity* owner) override;
		virtual void CleanupComponent_Internal(Component * obj) override;
		virtual void CloneComponent_Internal(Component * destination, const Component * source) override;
		JE_System_Behaviour_CheckComponentOverride;

		virtual void OnSwapChainResize_Internal(Component* obj) override;

		void OnTransformMovabilityAboutToChange(Transform* transform, Transform::Movability newValue);
		Transform::Data& GetDataOfTransform(Transform* transform);

		JE_Inline void ProcessTransformDataDynamic(Transform::Data& data);
		JE_Inline void ProcessTransformDataStatic(Transform::Data& data);
		JE_Inline void ProcessTransformDataCommon(Transform::Data& data);

		JE_Inline void CreateUboTransform(Transform* transform);
		JE_Inline void CleanupUboTransform(Transform* transform);

		Util::ObjectPool<Transform::Data> _transformDataPerMovability[(size_t)Transform::Movability::ENUM_SIZE];
		bool _bNeedUpdateUbos = false;
	};
}