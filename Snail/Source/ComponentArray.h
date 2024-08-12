#pragma once

#include "EntityManager.h"

#include <array>
#include <unordered_map>

namespace Snail
{

	class ComponentArrayBase
	{
	public:

		virtual void removeDataIfPresent(EntityId id) = 0;
		
		virtual ~ComponentArrayBase();
	};

	template <typename T>
	class ComponentArray : public ComponentArrayBase
	{
		std::array<T, MAX_ENTITIES> allData;
		std::unordered_map<size_t, EntityId> indexToEntityId;
		std::unordered_map<EntityId, size_t> entityIdToIndex;

		size_t size = 0; // size of valid entries in component data array

	public:

		void addData(EntityId id, T data)
		{
			crashIf(id >= MAX_ENTITIES, "Entity with id " + toStr(id) + " is out of bounds");
			crashIf(size >= MAX_ENTITIES, Util::quote(typeid(T).name()) + " component array is full");

			indexToEntityId[size] = id;
			entityIdToIndex[id] = size;
			allData[size++] = data;
		}

		void removeData(EntityId id)
		{
			crashIf(!entityIdToIndex.count(id), "Entity " + toStr(id) + " does not have " + 
				Util::quote(typeid(T).name()) + " component");
			crashIf(!indexToEntityId.count(size), "Last index of " + Util::quote(typeid(T).name()) +
				" component array does not have corresponding entity");

			allData[entityIdToIndex.at(id)] = allData[--size]; // copy last element to deleted element
			entityIdToIndex.at(indexToEntityId.at(size)) = entityIdToIndex.at(id); // update last element's mapping
			indexToEntityId.at(entityIdToIndex.at(id)) = indexToEntityId.at(size); // update last element's mapping
			entityIdToIndex.erase(id); // delete deleted element's mapping
			indexToEntityId.erase(entityIdToIndex.at(id)); // delete deleted element's mapping
		}

		T &getData(EntityId id)
		{
			crashIf(!entityIdToIndex.count(id), "Entity " + toStr(id) + " does not have " +
				Util::quote(typeid(T).name()) + " component");

			return allData[entityIdToIndex.at(id)];
		}

		void removeDataIfPresent(EntityId id) override
		{
			if (entityIdToIndex.count(id))
				removeData(id);
		}

	};

}