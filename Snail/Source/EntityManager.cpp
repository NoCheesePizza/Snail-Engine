#include "EntityManager.h"
#include "Timer.h"
#include "Core.h"

#include <iterator>

namespace Snail
{

	void EntityManager::init()
	{
		for (EntityId i = 0; i < MAX_ENTITIES; ++i)
			usableIds.push(i);
	}

	void EntityManager::update()
	{

	}

	void EntityManager::free()
	{

	}

	EntityId EntityManager::addEntity()
	{
		crashIf(usableIds.empty(), "Max entities reached");
		EntityId newId = usableIds.front();
		usedIds.insert(newId);
		usableIds.pop();
		return newId;
	}

	void EntityManager::removeEntity(EntityId id)
	{
		crashIf(usableIds.size() == MAX_ENTITIES, "No entities to remove");
		usedIds.erase(id);
		usableIds.push(id);
		gs(Time)->stopTimer(id);
	}

	bool EntityManager::isEntityAlive(EntityId id)
	{
		return usedIds.count(id);
	}

	void EntityManager::setSignature(EntityId id, Signature signature)
	{
		crashIf(id >= MAX_ENTITIES, "Entity with id " + toStr(id) + " is out of bounds");
		entities[id] = signature;
	}

	Signature EntityManager::getSignature(EntityId id)
	{
		crashIf(id >= MAX_ENTITIES, "Entity with id " + toStr(id) + " is out of bounds");
		return entities[id];
	}

}