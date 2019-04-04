#pragma once

#include "Util/Property.h"
#include "Util/ObjectPool.h"

#define JE_GetSystem() ::GOM::System::Get()

#if !defined(NDEBUG)
#define JE_BEHAVIOUR_CHECK_COMPONENT 1
#define JE_System_Behaviour_CheckComponent virtual void CheckComponent(const Component* obj) { JE_Assert(obj); }
#define JE_System_Behaviour_CheckComponentOverride virtual void CheckComponent(const Component* obj) override
#else
#define JE_BEHAVIOUR_CHECK_COMPONENT 0
#define JE_System_Behaviour_CheckComponent void CheckObject(const Component* obj) { }
#define JE_System_Behaviour_CheckComponentOverride void CheckObject(const Component* obj) override { }
#endif

#define JE_System_Component_Body_Declaration(BehaviourType, ComponentType)																\
	public:																																\
		static JE_Inline BehaviourType* GetBehaviour() { return JE_GetSystem()->GetBehaviour<BehaviourType>(); }						\
		JE_Inline virtual Behaviour* GetMyBehaviour() override																			\
			{ return reinterpret_cast<Behaviour*>(JE_GetSystem()->GetBehaviour<BehaviourType>()); }										\
	private:																															\
		friend class BehaviourType;

#define JE_System_Behaviour_Body_Declaration(BehaviourType, ComponentType)																\
	public:																																\
		static JE_Inline ComponentType* ComponentCast(Component* obj) { return static_cast<ComponentType*>(obj); }						\
		static JE_Inline const ComponentType* ComponentCast(const Component* obj) { return static_cast<const ComponentType*>(obj); }	\
																																		\
	private:																															\
		friend class ComponentType;

namespace GOM
{
	class Behaviour;
	class System;
	class Entity;

	class Component : public Util::NullType
	{
	public:

		JE_Inline virtual Behaviour* GetMyBehaviour() = 0;
		JE_Inline Entity* GetOwner() { return _owner; }
		JE_Inline const Entity* GetOwner() const { return _owner; }

	public:

	// All assignable data should be public.

	protected:
		Component();
		Component(const Component& copy);
		Component(const Component&& move);
		Component& operator=(const Component& copy);
		virtual ~Component();

		Entity* _owner = nullptr;

		friend class Behaviour;
		friend class Entity;
	};

	// Here can be passed any data that is crucial to be known right at object construction time.
	class ComponentConstructionParameters : public Util::NullType { };

	class Behaviour : public Util::NullType
	{
	public:

		virtual void Update() = 0;
		virtual void Draw() = 0;

		void CleanupRemainingObjects();

		Component* ConstructComponent(const ComponentConstructionParameters* constructionParam = nullptr);
		void InitializeComponent(Component* obj, Entity* owner);	// Initialize object which should have all fields assigned.
		void CleanupComponent(Component* obj);
		Component* CloneComponent(const Component* source);

		void OnSwapChainResize();

	protected:
		Behaviour() { }
		Behaviour(const Behaviour& copy) = delete;
		Behaviour(const Behaviour&& move) = delete;
		Behaviour& operator=(const Behaviour& copy) = delete;
		virtual ~Behaviour() { }

		virtual Component* ConstructComponent_Internal(const ComponentConstructionParameters* constructionParam) = 0;
		virtual void InitializeComponent_Internal(Component* obj) = 0;
		virtual void BindComponentWithOwner(Component* obj, Entity* owner);
		virtual void CleanupComponent_Internal(Component* obj) = 0;
		virtual void CloneComponent_Internal(Component* destination, const Component* source) = 0;
		JE_System_Behaviour_CheckComponent;

		virtual void OnSwapChainResize_Internal(Component* obj) { }

		typedef std::vector<Component*> ComponentCollection;

		ComponentCollection _componentsAll;
		bool _bIsPersistent = false;
		bool _bActiveFlag = false;

		friend class System;
	};

	class System
	{
	public:

		System() { }
		System(const System&) = delete;
		System(const System&&) = delete;
		System& operator=(const System&) = delete;
		~System() { }

		// TODO: This is only a temporary measure.
		static System* Get();

		void Initialize();
		void Cleanup();
		void Update();

		void OnSwapChainResize();

		void DeactivateBehaviourIfEmpty(Behaviour* behaviour);

		template <class BehaviourType> BehaviourType* GetBehaviour()
		{
			static BehaviourType behaviourInstance;
			if (!behaviourInstance._bActiveFlag)
			{
				_activeBehaviours.push_back(&behaviourInstance);
				behaviourInstance._bActiveFlag = true;
			}
			return &behaviourInstance;
		}

	private:

		void InitBaseBehaviours();


		typedef std::vector<Behaviour*> BehaviourCollection;

		BehaviourCollection _activeBehaviours;
	};
}