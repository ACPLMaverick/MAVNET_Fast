#pragma once

#include "System.h"

namespace GOM
{
	class TransformBehaviour;

	class Transform : public Component
	{
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

		void OnMovabilityChanged();

	public:

		Util::Property<Movability, Transform, &Transform::OnMovabilityChanged> PropMovability;

	protected:

		Transform()
			: Component()
			, PropMovability(Movability::Static)
		{
		}

		Util::ObjectPool<Data>::Index _data;

		friend class TransformBehaviour;
	};

	class TransformBehaviour : public Behaviour
	{
		JE_System_Behaviour_Body_Declaration(TransformBehaviour, Transform)

	public:

		// Inherited via Behaviour
		virtual void Update() override;
		virtual void Draw() override;

	protected:

		virtual Component * ConstructObject_Internal() override;
		virtual void InitializeObject_Internal(Component * obj) override;
		virtual void CleanupObject_Internal(Component * obj) override;
		virtual void CloneObject_Internal(Component * destination, const Component * source) override;
		JE_System_Behaviour_CheckObjectOverride;


		Util::ObjectPool<Transform::Data> _transformDataPool;
	};
}