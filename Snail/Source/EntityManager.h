#pragma once

#include "System.h"
#include "Utility.h"
#include "Components.h"

#include <queue>
#include <unordered_set>
#include <array>

namespace Snail
{

	class EntityManager : public System
	{
		std::queue<EntityId> usableIds;
		std::unordered_set<EntityId> usedIds; // live entity ids
		std::array<Signature, MAX_ENTITIES> entities; // all entities

	public:

		void init() override;
		void update() override;
		void free() override;

		EntityId addEntity();
		void removeEntity(EntityId id);
		bool isEntityAlive(EntityId id);
		void setSignature(EntityId id, Signature signature);
		Signature getSignature(EntityId id);
	};

}