#include "System.h"

#include "Core/HelloTriangle.h"
#include "Drawable.h"

namespace GOM
{
	SystemObject::SystemObject()
	{
	}

	SystemObject::SystemObject(const SystemObject & copy)
	{
	}

	SystemObject::SystemObject(const SystemObject && move)
	{
	}

	SystemObject & SystemObject::operator=(const SystemObject & copy)
	{
		return *this;
	}

	SystemObject::~SystemObject()
	{
	}


	void SystemBehaviour::CleanupRemainingObjects()
	{
		for (SystemObject* obj : _objectsAll)
		{
			CheckObject(obj);
			CleanupObject_Internal(obj);
			delete obj;
		}
		_objectsAll.clear();
		_bActiveFlag = false;
	}

	SystemObject * SystemBehaviour::ConstructObject()
	{
		return ConstructObject_Internal();
	}

	void SystemBehaviour::InitializeObject(SystemObject* obj)
	{
		JE_Assert(obj != nullptr);
		CheckObject(obj);
		InitializeObject_Internal(obj);
		_objectsAll.push_back(obj);
	}

	void SystemBehaviour::CleanupObject(SystemObject * obj)
	{
		JE_Assert(obj != nullptr);

		CheckObject(obj);
		CleanupObject_Internal(obj);

		delete obj;
		auto it = std::find(_objectsAll.begin(), _objectsAll.end(), obj);
		JE_Assert(it != _objectsAll.end());
		_objectsAll.erase(it);

		JE_GetApp()->GetSystem()->DeactivateBehaviourIfEmpty(this);
	}

	SystemObject * SystemBehaviour::CloneObject(const SystemObject * source)
	{
		CheckObject(source);
		SystemObject* newObject = ConstructObject();

		CloneObject_Internal(newObject, source);

		_objectsAll.push_back(newObject);
		return newObject;
	}


	void System::Initialize()
	{
		InitBaseBehaviours();
	}

	void System::Cleanup()
	{
		for (SystemBehaviour* behaviour : _activeBehaviours)
		{
			behaviour->CleanupRemainingObjects();
		}
		_activeBehaviours.clear();
	}

	void System::Update()
	{
		// TODO: May make this multithreaded.

		for (SystemBehaviour* behaviour : _activeBehaviours)
		{
			behaviour->Update();
		}
	}

	void System::Draw()
	{
		// TODO: May make this multithreaded.

		for (SystemBehaviour* behaviour : _activeBehaviours)
		{
			behaviour->Draw();
		}
	}

	void System::DeactivateBehaviourIfEmpty(SystemBehaviour * behaviour)
	{
		if (behaviour->_bIsPersistent || behaviour->_objectsAll.size() > 0)
		{
			return;
		}

		behaviour->_bActiveFlag = false;
		auto it = std::find(_activeBehaviours.begin(), _activeBehaviours.end(), behaviour);
		JE_Assert(it != _activeBehaviours.end());
		_activeBehaviours.erase(it);
	}

	void System::InitBaseBehaviours()
	{
		GetBehaviour<DrawableBehaviour>()->_bIsPersistent = true;
	}
}