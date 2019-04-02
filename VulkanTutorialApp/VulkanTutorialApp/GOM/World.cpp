#include "World.h"

#include "Core/HelloTriangle.h"

namespace GOM
{

	World::World()
	{

	}

	World::~World()
	{

	}

	void World::Initialize()
	{
		_globalParams.Initialize();

		// TODO: For now hard-coded stuff, will be deserialized in the future.

		*_globalParams.GetClearColorForEdit() = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

		glm::vec3 col(1.0f, 1.0f, 1.0f);
		//glm::vec3 dir(-1.0f, -0.5f, 0.2f);
		glm::vec3 dir(*JE_GetApp()->GetCamera()->GetTarget() - *JE_GetApp()->GetCamera()->GetPosition());
		dir = glm::normalize(dir);
		_globalParams.GetSunLightForEdit()->Initialize
		(
			&col,
			&dir
		);

		col.r = _globalParams.GetClearColor()->r;
		col.g = _globalParams.GetClearColor()->g;
		col.b = _globalParams.GetClearColor()->b;
		_globalParams.GetFogForEdit()->Initialize(&col, 20.0f, 50.0f);
	}

	void World::Cleanup()
	{
		for (Entity* entity : _entities)
		{
			entity->Cleanup();
			delete entity;
		}
		_entities.clear();

		_globalParams.Cleanup();
	}

	void World::Update()
	{
		_globalParams.Update();
	}

	Entity* World::AddEntity()
	{
		_entities.push_back(new Entity());

		Entity* entity = _entities.back();
		entity->Initialize();

		return entity;
	}

	void World::RemoveEntity(Entity* entity)
	{
		JE_Assert(entity);

		EntityCollection::iterator it = std::find(_entities.begin(), _entities.end(), entity);
		JE_Assert(it != _entities.end());

		(*it)->Cleanup();

		_entities.erase(it);
	}

	bool World::HasEntity(Entity* entity) const
	{
		JE_Assert(entity);
		return std::find(_entities.begin(), _entities.end(), entity) != _entities.end();
	}
}