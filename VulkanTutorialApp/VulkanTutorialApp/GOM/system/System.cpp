#include "System.h"

#include "Core/HelloTriangle.h"
#include "Drawable.h"

namespace GOM
{
	SystemDataObject::SystemDataObject()
	{
	}

	SystemDataObject::SystemDataObject(const SystemDataObject & copy)
	{
	}

	SystemDataObject::SystemDataObject(const SystemDataObject && move)
	{
	}

	SystemDataObject & SystemDataObject::operator=(const SystemDataObject & copy)
	{
		return *this;
	}

	SystemDataObject::~SystemDataObject()
	{
	}


	void SystemBehaviour::CleanupRemainingObjects()
	{
		for (SystemDataObject* obj : _objectsAll)
		{
			CleanupObject_Internal(obj);
			delete obj;
		}
		_objectsAll.clear();
		_bActiveFlag = false;
	}

	SystemDataObject * SystemBehaviour::ConstructObject()
	{
		return ConstructObject_Internal();
	}

	void SystemBehaviour::InitializeObject(SystemDataObject* obj)
	{
		JE_Assert(obj != nullptr);
		InitializeObject_Internal(obj);
		_objectsAll.push_back(obj);
	}

	void SystemBehaviour::CleanupObject(SystemDataObject * obj)
	{
		JE_Assert(obj != nullptr);

		CleanupObject_Internal(obj);

		delete obj;
		auto it = std::find(_objectsAll.begin(), _objectsAll.end(), obj);
		JE_Assert(it != _objectsAll.end());
		_objectsAll.erase(it);

		JE_GetApp()->GetSystem()->DeactivateBehaviourIfEmpty(this);
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