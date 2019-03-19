#pragma once

#if !defined(NDEBUG)
#define JE_System_Behaviour_CheckObject virtual void CheckObject(const SystemObject* obj) = 0
#define JE_System_Behaviour_CheckObjectOverride virtual void CheckObject(const SystemObject* obj) override
#else
#define JE_System_Behaviour_CheckObject void CheckObject(const SystemObject* obj) { }
#define JE_System_Behaviour_CheckObjectOverride void CheckObject(const SystemObject* obj) { }
#endif

#define JE_System_Behaviour_Body_Declaration(Type, ObjectType) \
	public: \
		static JE_Inline ObjectType* ObjectCast(SystemObject* obj) { return reinterpret_cast<ObjectType*>(obj); } \
		static JE_Inline const ObjectType* ObjectCast(const SystemObject* obj) { return reinterpret_cast<const ObjectType*>(obj); } \
	private:

namespace GOM
{
	class SystemBehaviour;
	class System;

	class SystemObject
	{
	public:

	// All assignable data should be public.

	protected:
		SystemObject();
		SystemObject(const SystemObject& copy);
		SystemObject(const SystemObject&& move);
		SystemObject& operator=(const SystemObject& copy);
		virtual ~SystemObject();

		friend class SystemBehaviour;
	};

	class SystemBehaviour
	{
	public:

		virtual void Update() = 0;
		virtual void Draw() = 0;

		void CleanupRemainingObjects();

		SystemObject* ConstructObject();
		void InitializeObject(SystemObject* obj);	// Initialize object which should have all fields assigned.
		void CleanupObject(SystemObject* obj);
		SystemObject* CloneObject(const SystemObject* source);

	protected:
		SystemBehaviour() { }
		SystemBehaviour(const SystemBehaviour& copy) = delete;
		SystemBehaviour(const SystemBehaviour&& move) = delete;
		SystemBehaviour& operator=(const SystemBehaviour& copy) = delete;
		virtual ~SystemBehaviour() { }

		virtual SystemObject* ConstructObject_Internal() = 0;
		virtual void InitializeObject_Internal(SystemObject* obj) = 0;
		virtual void CleanupObject_Internal(SystemObject* obj) = 0;
		virtual void CloneObject_Internal(SystemObject* destination, const SystemObject* source) = 0;
		JE_System_Behaviour_CheckObject;


		typedef std::vector<SystemObject*> ObjectCollection;

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

		void DeactivateBehaviourIfEmpty(SystemBehaviour* behaviour);

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


		typedef std::vector<SystemBehaviour*> BehaviourCollection;

		BehaviourCollection _activeBehaviours;
	};
}