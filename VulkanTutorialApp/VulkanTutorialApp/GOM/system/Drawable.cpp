#include "Drawable.h"

namespace GOM
{
	void DrawableBehaviour::Update()
	{
		// TODO
	}

	void DrawableBehaviour::Draw()
	{
		// TODO
	}

	SystemDataObject * DrawableBehaviour::ConstructObject_Internal()
	{
		return new DrawableObject();
	}

	void DrawableBehaviour::InitializeObject_Internal(SystemDataObject * objAbstract)
	{
		DrawableObject* obj = ObjectCast(objAbstract);
		// TODO
	}

	void DrawableBehaviour::CleanupObject_Internal(SystemDataObject * objAbstract)
	{
		DrawableObject* obj = ObjectCast(objAbstract);
		// TODO
	}

#if !defined(NDEBUG)
	void DrawableBehaviour::CheckObject(SystemDataObject* obj)
	{
		JE_Assert(obj != nullptr);
		JE_Assert(ObjectCast(obj)->PropMesh != nullptr);
		JE_Assert(ObjectCast(obj)->PropMaterial != nullptr);
	}
#endif
}