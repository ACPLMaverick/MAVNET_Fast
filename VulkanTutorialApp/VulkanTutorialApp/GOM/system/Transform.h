#pragma once

#include "System.h"

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

		struct Data
		{
			glm::mat4 WorldMatrix = glm::mat4(1.0f);
			glm::vec3 Position = glm::vec3(0.0f);
			glm::vec3 Rotation = glm::vec3(0.0f);
			glm::vec3 Scale = glm::vec3(1.0f);
		};

	protected:

		void OnMovabilityAboutToChange(const Movability& oldValue, const Movability& newValue);

	public:

		Util::Property<Movability, Transform, nullptr, &Transform::OnMovabilityAboutToChange> PropMovability;

		const glm::mat4& GetWorldMatrix() { return GetData().WorldMatrix; }
		glm::vec3& GetPosition() { return GetData().Position; }
		glm::vec3& GetRotation() { return GetData().Rotation; }
		glm::vec3& GetScale() { return GetData().Scale; }

		void SetPosition(const glm::vec3& position) { GetData().Position = position; }
		void SetRotation(const glm::vec3& rotation) { GetData().Rotation = rotation; }
		void SetScale(const glm::vec3& scale) { GetData().Scale = scale; }

		void AddPosition(const glm::vec3& positionOffset) { GetData().Position += positionOffset; }
		void AddRotation(const glm::vec3& rotationOffset) { GetData().Rotation += rotationOffset; }
		void AddScale(const glm::vec3& scaleOffset) { GetData().Scale += scaleOffset; }

	protected:

		Transform(Movability movability)
			: Component()
			, _indexData(Util::ObjectPool<Data>::BAD_INDEX)
			, PropMovability(movability)
		{
		}

		Data& GetData();

		Util::ObjectPool<Data>::Index _indexData;

		friend class TransformBehaviour;
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
		virtual void CleanupComponent_Internal(Component * obj) override;
		virtual void CloneComponent_Internal(Component * destination, const Component * source) override;

		void OnTransformMovabilityAboutToChange(Transform* transform, Transform::Movability newValue);
		Transform::Data& GetDataOfTransform(Transform* transform);

		JE_Inline void ProcessTransformData(Transform::Data& data);

		Util::ObjectPool<Transform::Data> _transformDataPerMovability[(size_t)Transform::Movability::ENUM_SIZE];
	};
}