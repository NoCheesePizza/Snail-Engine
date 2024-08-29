#include "EntityManager.h"
#include "ComponentManager.h"
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

	const std::unordered_set<EntityId> &EntityManager::getEntityIds()
	{
		return usedIds;
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

	bool EntityManager::hasComponent(EntityId entity, CompId comp)
	{
		return getSignature(entity).test(comp);
	}

	bool EntityManager::hasAnyComponents(EntityId entity, const std::vector<std::string> &comps)
	{
		Signature toCompare;
		for (const std::string &comp : comps)
			toCompare.set(gs(ComponentManager)->getCompId(comp));
		return (toCompare | getSignature(entity)) == Signature().set();
	}

	bool EntityManager::hasAllComponents(EntityId entity, const std::vector<std::string> &comps)
	{
		Signature toCompare;
		for (const std::string &comp : comps)
			toCompare.set(gs(ComponentManager)->getCompId(comp));
		return (toCompare & getSignature(entity)) == toCompare;
	}

}