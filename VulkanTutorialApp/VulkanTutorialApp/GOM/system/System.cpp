#include "System.h"

#include "Core/HelloTriangle.h"
#include "Drawable.h"

namespace GOM
{
	Component::Component()
	{
	}

	Component::Component(const Component & copy)
	{
	}

	Component::Component(const Component && move)
	{
	}

	Component & Component::operator=(const Component & copy)
	{
		return *this;
	}

	Component::~Component()
	{
	}


	void Behaviour::CleanupRemainingObjects()
	{
		for (Component* obj : _objectsAll)
		{
			CheckObject(obj);
			CleanupObject_Internal(obj);
			delete obj;
		}
		_objectsAll.clear();
		_bActiveFlag = false;
	}

	Component * Behaviour::ConstructObject()
	{
		return ConstructObject_Internal();
	}

	void Behaviour::InitializeObject(Component* obj)
	{
		JE_Assert(obj != nullptr);
		CheckObject(obj);
		InitializeObject_Internal(obj);
		_objectsAll.push_back(obj);
	}

	void Behaviour::CleanupObject(Component * obj)
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

	Component * Behaviour::CloneObject(const Component * source)
	{
		CheckObject(source);
		Component* newObject = ConstructObject();

		CloneObject_Internal(newObject, source);

		_objectsAll.push_back(newObject);
		return newObject;
	}

	void Behaviour::OnSwapChainResize()
	{
		for (Component* obj : _objectsAll)
		{
			OnSwapChainResize_Internal(obj);
		}
	}


	void System::Initialize()
	{
		InitBaseBehaviours();
	}

	void System::Cleanup()
	{
		for (Behaviour* behaviour : _activeBehaviours)
		{
			behaviour->CleanupRemainingObjects();
		}
		_activeBehaviours.clear();
	}

	void System::Update()
	{
		// TODO: May make this multithreaded.

		for (Behaviour* behaviour : _activeBehaviours)
		{
			behaviour->Update();
		}
	}

	void System::Draw()
	{
		// TODO: May make this multithreaded.

		for (Behaviour* behaviour : _activeBehaviours)
		{
			behaviour->Draw();
		}
	}

	void System::OnSwapChainResize()
	{
		for (Behaviour* behaviour : _activeBehaviours)
		{
			behaviour->OnSwapChainResize();
		}
	}

	void System::DeactivateBehaviourIfEmpty(Behaviour * behaviour)
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