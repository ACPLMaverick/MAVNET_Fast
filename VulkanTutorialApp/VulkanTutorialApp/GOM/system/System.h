#pragma once

#if !defined(NDEBUG)
#define JE_System_Behaviour_CheckObject virtual void CheckObject(SystemDataObject* obj) = 0
#define JE_System_Behaviour_CheckObjectOverride virtual void CheckObject(SystemDataObject* obj) override
#else
#define JE_System_Behaviour_CheckObject void CheckObject(SystemDataObject* obj) { }
#define JE_System_Behaviour_CheckObjectOverride void CheckObject(SystemDataObject* obj) { }
#endif

#define JE_System_Behaviour_Body_Declaration(Type, ObjectType) \
	public: \
		static inline ObjectType* ObjectCast(SystemDataObject* obj) { return reinterpret_cast<ObjectType*>(obj); } \
	private:

namespace GOM
{
	class SystemBehaviour;
	class System;

	class SystemDataObject
	{
	public:

	// All assignable data should be public.

	protected:
		SystemDataObject();
		SystemDataObject(const SystemDataObject& copy);
		SystemDataObject(const SystemDataObject&& move);
		SystemDataObject& operator=(const SystemDataObject& copy);
		virtual ~SystemDataObject();

		friend class SystemBehaviour;
	};

	class SystemBehaviour
	{
	public:

		virtual void Update() = 0;
		virtual void Draw() = 0;

		void CleanupRemainingObjects();

		SystemDataObject* ConstructObject();
		void InitializeObject(SystemDataObject* obj);	// Initialize object which should have all fields assigned.
		void CleanupObject(SystemDataObject* obj);

	protected:
		SystemBehaviour() { }
		SystemBehaviour(const SystemBehaviour& copy) = delete;
		SystemBehaviour(const SystemBehaviour&& move) = delete;
		SystemBehaviour& operator=(const SystemBehaviour& copy) = delete;
		virtual ~SystemBehaviour() { }

		virtual SystemDataObject* ConstructObject_Internal() = 0;
		virtual void InitializeObject_Internal(SystemDataObject* obj) = 0;
		virtual void CleanupObject_Internal(SystemDataObject* obj) = 0;
		JE_System_Behaviour_CheckObject;


		typedef std::vector<SystemDataObject*> ObjectCollection;

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