#pragma once

#include "Util/Property.h"
#include "Util/ObjectPool.h"


#if !defined(NDEBUG)
#define JE_BEHAVIOUR_CHECK_OBJECT 1
#define JE_System_Behaviour_CheckObject virtual void CheckObject(const Component* obj) = 0
#define JE_System_Behaviour_CheckObjectOverride virtual void CheckObject(const Component* obj) override
#else
#define JE_BEHAVIOUR_CHECK_OBJECT 0
#define JE_System_Behaviour_CheckObject void CheckObject(const Component* obj) { }
#define JE_System_Behaviour_CheckObjectOverride void CheckObject(const Component* obj) { }
#endif

#define JE_System_Behaviour_Body_Declaration(Type, ObjectType) \
	public: \
		static JE_Inline ObjectType* ObjectCast(Component* obj) { return reinterpret_cast<ObjectType*>(obj); } \
		static JE_Inline const ObjectType* ObjectCast(const Component* obj) { return reinterpret_cast<const ObjectType*>(obj); } \
	private:

namespace GOM
{
	class Behaviour;
	class System;

	class Component : public Util::NullType
	{
	public:

	// All assignable data should be public.

	protected:
		Component();
		Component(const Component& copy);
		Component(const Component&& move);
		Component& operator=(const Component& copy);
		virtual ~Component();

		friend class Behaviour;
	};

	class Behaviour : public Util::NullType
	{
	public:

		virtual void Update() = 0;
		virtual void Draw() = 0;

		void CleanupRemainingObjects();

		Component* ConstructObject();
		void InitializeObject(Component* obj);	// Initialize object which should have all fields assigned.
		void CleanupObject(Component* obj);
		Component* CloneObject(const Component* source);

		void OnSwapChainResize();

	protected:
		Behaviour() { }
		Behaviour(const Behaviour& copy) = delete;
		Behaviour(const Behaviour&& move) = delete;
		Behaviour& operator=(const Behaviour& copy) = delete;
		virtual ~Behaviour() { }

		virtual Component* ConstructObject_Internal() = 0;
		virtual void InitializeObject_Internal(Component* obj) = 0;
		virtual void CleanupObject_Internal(Component* obj) = 0;
		virtual void CloneObject_Internal(Component* destination, const Component* source) = 0;
		JE_System_Behaviour_CheckObject;

		virtual void OnSwapChainResize_Internal(Component* obj) { }

		typedef std::vector<Component*> ObjectCollection;

		ObjectCollection _objectsAll;
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

		void Initialize();
		void Cleanup();
		void Update();
		void Draw();

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